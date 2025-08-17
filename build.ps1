$outputExe = "main.exe"

# Compile using cl.exe
# /Zi -> generate debug info
# /nologo -> suppress banner
# /Fe -> specify output executable
# W3 -> Level 3 warnings
# WX -> warnings are errors
# SDL -> safety warnings
$command = "cl.exe /sdl /W3 /WX /Zi /nologo /Fe$outputExe *.c"

Write-Host "Running: $command"
Invoke-Expression $command

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