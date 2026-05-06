`python3 -m venv v`

`source v/bin/activate`

`pip install PySide6 pyinstaller`

`g++ -O3 algo.cpp -o algo`

`pyinstaller --onefile --add-binary "algo:." --windowed GUI.py`

`mkdir -p AppDir/usr/bin`

`cp dist/GUI AppDir/usr/bin/`

`wget "https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20251107-1/linuxdeploy-x86_64.AppImage"`

`wget "https://github.com/AppImage/type2-runtime/releases/download/continuous/runtime-x86_64"`

`chmod +x linuxdeploy-x86_64.AppImage`

`export ARCH=x86_64`

`cp runtime-x86_64 ./runtime`

`chmod +x runtime`

`export APPIMAGETOOL_RUN_FILE=$(pwd)/runtime`

`./linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage`

`chmod +x WagnerFischer-x86_64.AppImage`

`./WagnerFischer-x86_64.AppImage`