#!/bin/bash

TOOLS_DIR="../archive/RemoteTasks"
MANAGER="$TOOLS_DIR/Portable.RemoteTasks.Manager.exe"

archName="vm"
defFile="vm.target.pdsl"
asmListing="program.asm"

login="509092"
password="3e27905a-827f-43c8-8274-70ad7e088ab5"

echo "===================================="
echo "VM Architecture Build Script"
echo "===================================="
echo "Architecture: $archName"
echo "Definition file: $defFile"
echo "Assembly listing: $asmListing"
echo "Manager: $MANAGER"
echo "Login: $login"
echo "===================================="
echo ""

mkdir -p util

echo "[1/3] Assembling $asmListing..."
mono "$MANAGER" -ul $login -up $password -w -id -s Assemble asmListing $asmListing definitionFile $defFile archName $archName > util/tmp.txt
id=$(cat util/tmp.txt 2>/dev/null || echo "")

if [ -z "$id" ]; then
    echo "ERROR: Assembly failed. Check util/tmp.txt for details"
    cat util/tmp.txt
    exit 1
fi

echo "TaskID: $id"
echo ""

echo "[2/3] Getting binary file..."
mono "$MANAGER" -ul $login -up $password -g $id
mono "$MANAGER" -ul $login -up $password -g $id -r out.ptptb > util/asmfile
echo "Binary file saved as: out.ptptb"
echo ""

echo "[3/3] Disassembling for verification..."
mono "$MANAGER" -ul $login -up $password -g $id -r disasmListing.txt > util/dissfile
echo "Disassembly saved as: disasmListing.txt"
echo ""

echo "===================================="
echo "Parameters for execution:"
echo "===================================="
echo "archName $archName \\"
echo "binaryFileToRun out.ptptb \\"
echo "codeRamBankName RAM \\"
echo "ipRegStorageName ip \\"
echo "finishMnemonicName HALT"
echo "===================================="
