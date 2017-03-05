@setlocal 
@call "%VS140COMNTOOLS%vsvars32.bat"
@for /R %%s in (*.sln) do (@call MSBuild %%s /t:Build /p:Configuration=Debug /p:Platform=x64 /fileLoggerParameters:LogFile=build.log;Verbosity=diagnostic;Encoding=UTF-8)
@endlocal