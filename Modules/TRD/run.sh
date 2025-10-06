configfile=$1
o2-ctf-reader-workflow --ctf-input Data.txt --onlyDet TRD |
o2-qc -b --config json://"$configfile" &> output.log 

#TRY2

#!/bin/bash

# configfile=$1
# ctf_file="Data.txt"
# dpl_output="trd-output.dpl"

# # # Load O2 environment
# # source ~/alice/sw/ubuntu2404_x86-64/O2/latest/env.sh

# # Run reader workflow and save to DPL file
# o2-ctf-reader-workflow --ctf-input "$ctf_file" --onlyDet TRD --pipeline-output "$dpl_output"

# # Run QC on the DPL file
# o2-qc -b --config json://"$configfile" --pipeline-input "$dpl_output" &> output.log

#TRY3
# #!/bin/bash

# # Short TRD QC runner for finite CTF files

# configfile=$1
# ctf_file="Data.txt"

# # Make sure input CTF file exists
# if [ ! -f "$ctf_file" ]; then
#     echo "Error: CTF file '$ctf_file' not found."
#     exit 1
# fi

# # Run reader workflow and pipe safely to QC
# o2-ctf-reader-workflow --ctf-input "$ctf_file" --onlyDet TRD \
#     | o2-qc -b --config json://"$configfile" --ignore-broken-pipe &> output.log

# echo "QC run completed. See output.log for details."


#TRY4
# #!/bin/bash

# # TRD QC runner using o2-dpl-run to avoid broken pipe crashes

# configfile=$1
# ctf_file="Data.txt"

# # Check that input file exists
# if [ ! -f "$ctf_file" ]; then
#     echo "Error: CTF file '$ctf_file' not found."
#     exit 1
# fi

# echo "=== Running TRD QC workflow ==="

# o2-dpl-run --workflow "ctf-reader:ctf-reader --ctf-input $ctf_file --onlyDet TRD | o2-qc -b --config json://$configfile" &> output.log

# echo "QC run completed. See output.log for details."

#TRY 5
# #!/bin/bash

# # TRD QC runner for finite CTF files (offline mode)

# configfile=$1
# ctf_file="Data.txt"

# # Check that input CTF file exists
# if [ ! -f "$ctf_file" ]; then
#     echo "Error: CTF file '$ctf_file' not found."
#     exit 1
# fi

# echo "=== Running TRD QC offline ==="

# # Run QC directly on the input file
# o2-qc -b --config json://"$configfile" --input "$ctf_file" &> output.log

# echo "QC run completed. See output.log for details."

#TRY6
# #!/bin/bash

# # TRD QC streaming workflow runner for your O² version

# configfile=$1
# ctf_file="Data.txt"

# # Check input file
# if [ ! -f "$ctf_file" ]; then
#     echo "Error: CTF file '$ctf_file' not found."
#     exit 1
# fi

# echo "=== Running TRD QC streaming workflow ==="

# # Launch the DPL workflow safely
# o2-dpl-workflow -w "ctf-reader:ctf-reader --ctf-input $ctf_file --onlyDet TRD | o2-qc -b --config json://$configfile" &> output.log

# echo "QC run completed. See output.log for details."



