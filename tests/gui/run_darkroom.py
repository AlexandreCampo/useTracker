#!/usr/bin/env python3
"""Exercise Darkroom through the GUI, with disposable video and output files."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile
import xml.etree.ElementTree as ET


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("executable", type=Path)
    parser.add_argument("--output", type=Path, help="Directory for logs and screenshots")
    parser.add_argument("--onscreen", action="store_true", help="Use the desktop instead of SDL offscreen")
    args = parser.parse_args()
    executable = args.executable.resolve()
    root = Path(__file__).resolve().parents[2]
    output = (args.output or Path(tempfile.mkdtemp(prefix="usetracker-gui-"))).resolve()
    output.mkdir(parents=True, exist_ok=True)
    # A dedicated directory prevents output tests from touching research data.
    with tempfile.TemporaryDirectory(prefix="fixture-", dir=output) as temporary:
        work = Path(temporary)
        video, image = work / "source.mp4", work / "source.png"
        subprocess.run(["ffmpeg", "-v", "error", "-f", "lavfi", "-i",
                        "testsrc2=size=640x360:rate=25", "-t", "4", "-c:v", "mpeg4",
                        "-q:v", "3", str(video)], check=True)
        subprocess.run(["ffmpeg", "-v", "error", "-i", str(video), "-frames:v", "1",
                        "-update", "1", str(image)], check=True)
        script = (root / "tests/gui/darkroom.script").read_text()
        script = script.replace("@OUT@", str(output)).replace("@IMAGE@", str(image))
        script_path = work / "workflow.script"
        script_path.write_text(script)
        env = os.environ.copy()
        if not args.onscreen:
            env["SDL_VIDEODRIVER"] = "offscreen"
            env["LIBGL_ALWAYS_SOFTWARE"] = "1"
        log_path = output / "workflow.log"
        with log_path.open("w") as log:
            result = subprocess.run([str(executable), "-i", str(video), "-p",
                                     str(root / "examples/fish_pattern_tracking.xml"),
                                     "--test", str(script_path)], cwd=work, env=env,
                                    stdout=log, stderr=subprocess.STDOUT, timeout=90)
        text = log_path.read_text()
        if result.returncode or "GUI CHECK FAILED" in text or "Pipeline plugin error" in text:
            raise RuntimeError(f"GUI workflow failed; see {log_path}\n{text[-3000:]}")
        tree = ET.parse(output / "darkroom-analysis.xml")
        pipeline = tree.find("./Configuration/Pipeline")
        names = [next(iter(stage)).tag for stage in pipeline]
        assert names == ["BackgroundDiffKnn", "Erosion", "PatternTracker"], names
        assert float(pipeline.find("./Plugin_0/BackgroundDiffKnn/Threshold").text) == 123

        empty = work / "empty.script"
        empty.write_text(f"resize 1440 900\nwait 8\nexpect source empty\n"
                         f"shot {output / 'welcome.png'}\nresize 960 640\nwait 6\n"
                         f"shot {output / 'welcome-small.png'}\nquit\n")
        with (output / "welcome.log").open("w") as log:
            subprocess.run([str(executable), "--test", str(empty)], cwd=work, env=env,
                           stdout=log, stderr=subprocess.STDOUT, check=True, timeout=30)
        print(f"Passed {text.count('GUI check passed')} workflow checks and the empty-source check.")
        print(f"Saved XML retains the edited threshold and stage order. Artifacts: {output}")


if __name__ == "__main__":
    main()
