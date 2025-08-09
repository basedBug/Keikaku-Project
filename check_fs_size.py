Import("env")
import os

def print_fs_size(source, target, env):
    fs_bin = env.subst("$BUILD_DIR/littlefs.bin")
    
    if not os.path.exists(fs_bin):
        print("⚠️ Filesystem image not found at:", fs_bin)
        return

    # 1. Get the ACTUAL partition file being used in the build
    partition_file = env.GetProjectOption("board_build.partitions", "")
    if not partition_file:
        # Fallback to board config if not specified in platformio.ini
        partition_file = env.BoardConfig().get("build.partitions", "partitions.csv")
    
    # 2. Search paths in order of priority
    search_paths = [
        env.subst(f"$PROJECT_DIR/{partition_file}"),  # First try project dir
        env.subst(f"$PROJECT_DIR/{os.path.basename(partition_file)}"),  # Just filename
        os.path.join(env.PioPlatform().get_package_dir("framework-arduinoespressif32"), 
                   "tools", "partitions", partition_file),
        os.path.join(env.PioPlatform().get_package_dir("framework-arduinoespressif32"),
                   "tools", "partitions", "default.csv")
    ]

    fs_partition_size = 0
    used_partition_file = ""

    # Find filesystem partition size
    for pf in search_paths:
        if os.path.exists(pf):
            print(f"Checking partition file: {pf}")  # Debug output
            with open(pf, "r") as f:
                for line in f:
                    line = line.strip()
                    if line.startswith("#") or not line:
                        continue
                    parts = [p.strip() for p in line.split(",")]
                    if len(parts) >= 5 and ("spiffs" in parts[2] or "littlefs" in parts[2]):
                        try:
                            fs_partition_size = int(parts[4].strip(), 16)
                            used_partition_file = pf
                            break
                        except ValueError:
                            continue
            if fs_partition_size:
                break

    if not fs_partition_size:
        print("\n⚠️ Could not find filesystem partition size in:")
        for f in search_paths:
            print(f"  - {f}")
        print("\nPlease ensure your partition table contains a line like:")
        print("spiffs,data,spiffs,0x210000,0x1E0000")
        return

    fs_size = os.path.getsize(fs_bin)
    percent_used = (fs_size / fs_partition_size) * 100

    print("\n📁 Filesystem Usage:")
    print(f"  • Partition file: {os.path.basename(used_partition_file)}")
    print(f"  • Image size:    {fs_size} bytes ({fs_size/1024:.1f} KB)")
    print(f"  • Partition size: {fs_partition_size} bytes ({fs_partition_size/1024:.1f} KB)")
    print(f"  • Used:         [{'=' * int(percent_used/10)}{' ' * (10 - int(percent_used/10))}] {percent_used:.1f}%")

    if percent_used > 100:
        print("\n❌ CRITICAL: Filesystem exceeds partition by {:.1f}%!".format(percent_used-100))
        print("Remove {} bytes from your data/ directory".format(fs_size - fs_partition_size))

# This ensures the function is only attached after buildfs completes (only runs with buildfs)
env.AddPostAction("buildfs", print_fs_size)