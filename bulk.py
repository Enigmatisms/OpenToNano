import os
import sys

path_in = "fluid"
path_out = "inputs"

def execute_vdb2nvdb_commands_single(num_iterations: int):
    for index in range(1, num_iterations + 1):
        
        vdb_input   =  os.path.join(path_in, f"fluid_data_{index:04d}.vdb")
        nvdb_output =  os.path.join(path_out, f"density_{index:04d}.nvdb")
        command = f"./build/vdb2nvdb {vdb_input} {nvdb_output}"
        
        os.system(command)

def execute_vdb2nvdb_commands_double(num_iterations: int):
    for index in range(1, num_iterations + 1):
        
        vdb_input   =  os.path.join(path_in, f"fluid_data_{index:04d}.vdb")
        nvdb_output1 =  os.path.join(path_out, f"density_{index:04d}.nvdb")
        nvdb_output2 =  os.path.join(path_out, f"flame_{index:04d}.nvdb")
        command1 = f"./build/vdb2nvdb {vdb_input} {nvdb_output1} density"
        command2 = f"./build/vdb2nvdb {vdb_input} {nvdb_output2} flame"
        
        os.system(command1)
        os.system(command2)

if __name__ == "__main__":
    if len(sys.argv) > 2:
        path_in = sys.argv[1]
        path_out = sys.argv[2]
    execute_vdb2nvdb_commands_double(191)