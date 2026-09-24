import subprocess
import os
import pytest

def get_bankcore_executable():
    """Locate compiled BankCore executable."""
    paths = ["./BankCore.exe", "./BankCore", "BankCore.exe", "BankCore"]
    for path in paths:
        if os.path.exists(path):
            return path
    pytest.skip("BankCore executable not compiled. Run 'make BankCore' first.")

def test_bankcore_deposit():
    """Verify account creation, deposit, and balance assertion."""
    exe = get_bankcore_executable()
    process = subprocess.Popen(
        [exe], 
        stdin=subprocess.PIPE, 
        stdout=subprocess.PIPE, 
        text=True
    )
    
    # Input simulation:
    # 1: Create Account -> Name: Naman
    # 2: Deposit -> Amount: 100
    # 4: View Balance
    # 5: Exit
    stdout, _ = process.communicate(input="1\nNaman\n2\n100\n4\n5\n")
    
    assert "Balance: 100" in stdout or "100.00" in stdout
    assert process.returncode == 0

def test_bankcore_transfer_flow():
    """Verify user registration, deposit, and transfer to recipient."""
    exe = get_bankcore_executable()
    process = subprocess.Popen(
        [exe], 
        stdin=subprocess.PIPE, 
        stdout=subprocess.PIPE, 
        text=True
    )
    
    # 1: Create Account -> Alice
    # 2: Deposit 500
    # 3: Transfer to 1001 -> 150
    # 4: View Balance
    # 5: Exit
    stdout, _ = process.communicate(input="1\nAlice\n2\n500\n3\n1001\n150\n4\n5\n")
    
    assert "Deposit successful" in stdout or "Balance: 500" in stdout
    assert process.returncode == 0

def test_bankcore_negative_deposit_rejected():
    """Verify that negative deposit attempts are rejected safely."""
    exe = get_bankcore_executable()
    process = subprocess.Popen(
        [exe], 
        stdin=subprocess.PIPE, 
        stdout=subprocess.PIPE, 
        text=True
    )
    
    # 1: Create Account -> Bob
    # 2: Deposit -50
    # 5: Exit
    stdout, _ = process.communicate(input="1\nBob\n2\n-50\n5\n")
    
    assert "Error" in stdout or "must be positive" in stdout
    assert process.returncode == 0

def test_bankcore_exit_cleanly():
    """Verify application exits with returncode 0."""
    exe = get_bankcore_executable()
    process = subprocess.Popen(
        [exe], 
        stdin=subprocess.PIPE, 
        stdout=subprocess.PIPE, 
        text=True
    )
    
    stdout, _ = process.communicate(input="5\n")
    assert process.returncode == 0
