echo Y | del BCB5.0\Debug\*.*
echo Y | del BCB5.0\Release\*.*

echo Y | del BCCDEV1.2\Debug\*.*
echo Y | rd BCCDEV1.2\Debug
echo Y | del BCCDEV1.2\Release\*.*
echo Y | rd BCCDEV1.2\Release

echo Y | del MSVC6.0\Debug\*.*
echo Y | rd MSVC6.0\Debug
echo Y | del MSVC6.0\Release\*.*
echo Y | rd MSVC6.0\Release
echo Y | del MSVC6.0\*.ncb

echo Y | del MSVC7.1\Debug\*.*
echo Y | rd MSVC7.1\Debug
echo Y | del MSVC7.1\Release\*.*
echo Y | rd MSVC7.1\Release
echo Y | del MSVC7.1\*.ncb
