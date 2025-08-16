# build.ps1
# Compile all .c files in the current directory with cl.exe

# Set the output executable name
$outputExe = "main.exe"

# Compile using cl.exe
# /Zi -> generate debug info
# /EHsc -> standard C++ exception handling (ignored for C)
# /nologo -> suppress banner
# /Fe -> specify output executable
$command = "cl.exe /RTC1 /Zi /EHsc /nologo /Fe$outputExe *.c"

Write-Host "Running: $command"
Invoke-Expression $command

# Check if compilation succeeded
if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilation succeeded! Output: $outputExe"
} else {
    Write-Host "Compilation failed with exit code $LASTEXITCODE"
    exit
}

# cls
Write-Host "Running: ./main.exe"
& .\$outputExe

$exitCode = $LASTEXITCODE
Write-Host "Exit Code: $exitCode"