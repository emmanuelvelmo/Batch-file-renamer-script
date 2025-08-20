from cx_Freeze import setup, Executable

setup(name="Batch file renamer", executables=[Executable("Batch file renamer script.py")], options={"build_exe": {"excludes": ["tkinter"]}})
