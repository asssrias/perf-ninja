import sys
import subprocess
from subprocess import Popen
import os

def main():
    if len(sys.argv) != 4:
        print("Usage: python bench.py <lab_path> <use_llvm> <enable_profile>")
        sys.exit(1)
    
    lab_path = sys.argv[1]
    use_llvm = sys.argv[2].lower() in ('true', '1', 'yes', 'y')
    enable_profile = sys.argv[3].lower() in ('true', '1', 'yes', 'y')
    build_path = os.path.join(lab_path, "build")
    if not os.path.exists(build_path):
        os.makedirs(build_path)
    os.chdir(build_path)
    
    try:
        # if enable_profile and os.path.exists(build_path):
        #     print("Profile is enabled.")
        #     os.chdir(lab_path)
        #     os.remove(build_path)
        #     os.chdir(build_path)
        build_command = ["cmake", "-G Ninja", "-DCMAKE_CXX_COMPILER=clang++", "-DCMAKE_C_COMPILER=clang", "-DCMAKE_BUILD_TYPE=RelWithDebInfo"]
        if enable_profile:
            build_command.extend(["-DCMAKE_C_FLAGS=\"-g\"", "-DCMAKE_CXX_FLAGS=\"-g\""])
        
        if use_llvm:
            p = Popen("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat", shell=True, stdout = subprocess.PIPE)
            stdout, stderr = p.communicate()
            print(stdout.decode("utf-8"))
            build_command.extend([
                "-G", 
                "Ninja",
                "-DCMAKE_PREFIX_PATH=C:\\Program Files (x86)\\benchmark\\lib\\cmake\\benchmark",
                # "-DCMAKE_CXX_COMPILER=\'C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe\'", 
                # "-DCMAKE_C_COMPILER=\'C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe\'", 
                # "-DCMAKE_C_COMPILER_ID=MSVC",
                # "-DCMAKE_CXX_COMPILER_ID=MSVC",
                # "-DCMAKE_SYSTEM_NAME=Generic",
                
                # "-DCMAKE_PREFIX_PATH=C:\\Program Files (x86)\\benchmark\\lib\\cmake\\benchmark",
                # "-DCMAKE_CXX_COMPILER=clang++", 
                # "-DCMAKE_C_COMPILER=clang", 
                # "-DCMAKE_C_COMPILER_ID=Clang",
                # "-DCMAKE_CXX_COMPILER_ID=Clang",
                # "-DCMAKE_SYSTEM_NAME=Windows",
                # "--debug-trycompile",
                
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"])
        
        build_command.append(lab_path)
        print("Build directory not exists. Building...")
        subprocess.run(build_command, check=True)
        
        lab_command = ["cmake", "--build", build_path, "--config", "RelWithDebInfo", "--parallel", "8"]
        validate_command = ["cmake", "--build", build_path, "--config", "RelWithDebInfo", "--target", "validateLab"]
        benchmark_command = ["cmake", "--build", build_path, "--config", "RelWithDebInfo", "--target", "benchmarkLab"]
        
        print("Building lab...")
        subprocess.run(lab_command, check=True)
        print("Building validate...")
        subprocess.run(validate_command, check=True)
        print("Building benchmark...")
        subprocess.run(benchmark_command, check=True)
        print("Build completed successfully.")
        
        base_command = [os.path.join(build_path, "RelWithDebInfo\\lab.exe"), "--benchmark_min_time=1", "--benchmark_out_format=json", "--benchmark_out=baseline.json"]
        solution_command = [os.path.join(build_path, "RelWithDebInfo\\lab.exe"), "--benchmark_min_time=1", "--benchmark_out_format=json", "--benchmark_out=solution.json"]
        compare_command = ["python", "E:\\benchmark\\tools\\compare.py", "benchmarks", "baseline.json", "solution.json"]
        
        if not os.path.exists(build_path + "/baseline.json"):
            subprocess.run(base_command, check=True)
        subprocess.run(solution_command, check=True)
        subprocess.run(compare_command, check=True)
            
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while building: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()