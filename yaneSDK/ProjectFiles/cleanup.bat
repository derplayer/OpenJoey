echo Y | del BCB5.0\Debug\*.*
echo Y | del BCB5.0\Release\*.*
echo Y | del BCB5.0\lib\*.*

rem dummyのファイルを置いておかないと空のフォルダは
rem 圧縮できないかも知れん

echo dummy > BCB5.0\Debug\dummy.txt
echo dummy > BCB5.0\Release\dummy.txt
echo dummy > BCB5.0\lib\dummy.txt

echo Y | del BCCDEV1.2\Debug\*.*
echo Y | rd BCCDEV1.2\Debug
echo Y | del BCCDEV1.2\Release\*.*
echo Y | rd BCCDEV1.2\Release
echo Y | del BCCDEV1.2\lib\*.*

echo dummy > BCCDEV1.2\Debug\dummy.txt
echo dummy > BCCDEV1.2\Release\dummy.txt
echo dummy > BCCDEV1.2\lib\dummy.txt

echo Y | del MSVC6.0\Debug\*.*
echo Y | rd MSVC6.0\Debug
echo Y | del MSVC6.0\Release\*.*
echo Y | rd MSVC6.0\Release
echo Y | del MSVC6.0\lib\*.*
echo Y | del MSVC6.0\*.ncb

echo dummy > MSVC6.0\Debug\dummy.txt
echo dummy > MSVC6.0\Release\dummy.txt
echo dummy > MSVC6.0\lib\dummy.txt

echo Y | del MSVC7.1\Debug\*.*
echo Y | rd MSVC7.1\Debug
echo Y | del MSVC7.1\Release\*.*
echo Y | rd MSVC7.1\Release
echo Y | del MSVC7.1\lib\*.*
echo Y | del MSVC7.1\*.ncb

echo dummy > MSVC7.1\Debug\dummy.txt
echo dummy > MSVC7.1\Release\dummy.txt
echo dummy > MSVC7.1\lib\dummy.txt
