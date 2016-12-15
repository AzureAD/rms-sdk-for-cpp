$logFile = "..\rms-sdk-for-cpp\bin\tests\rmsUnitTestResults.txt"

$xmlOutputFile = "..\rms-sdk-for-cpp\bin\tests\rmsUnitTestResults.xml"
$result = ""


$result,$file = Get-Content $logFile
$result = $result + "`n<testsuites>"

if(Test-Path -Path $xmlOutputFile){
    Clear-Content $xmlOutputFile
}

foreach($line in $file){

    Write-Host $line

    if(!$line.startsWith("<?xml")){

        $result = $result + "`n`t"+$line;
    }
}
$result = $result + "`n</testsuites>"

Write-Host $result
Add-content $xmlOutputFile -value $result

$xml = [xml](Get-Content $xmlOutputFile)
foreach($testsuite in $xml.testsuites.testsuite){
    foreach($test in $testsuite.testcase){
        $test.name = $testsuite.name + "::" + $test.name
    }
}
 Set-Content $xmlOutputFile $xml.OuterXml


