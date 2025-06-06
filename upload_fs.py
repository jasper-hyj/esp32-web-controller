# upload_fs.py
Import("env")

def before_upload_fs(source, target, env):
    env.Execute("pio run --target uploadfs")

env.AddPreAction("upload", before_upload_fs)
env.AddPreAction("program", before_upload_fs)  # optional: handles manual programming
