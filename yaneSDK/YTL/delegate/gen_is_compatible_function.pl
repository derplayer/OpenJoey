#
#	Function Compatibility Testing Template Class Generator
#		programmed by ENRA		'03/06/25
#		reprogrammed by ENRA	'03/07/12	クラス名の変更(;´Д`)
#

if ($#ARGV < 0) {
  print "Usage: perl gen_is_compatible_function <number of arguments>\n";
  exit;
}

open OUT, ">is_compatible_function.hpp";

print OUT "/**\n";
print OUT " *	Function Compatibility Testing Template Class\n";
print OUT " *		programmed by ENRA(S.Takebata)		'03/07/06\n";
print OUT " *		reprogrammed by ENRA(S.Takebata)	'03/07/12	クラス名の変更(;´Д`)\n";
print OUT " *\n";
print OUT " *	一応オリジナルのアルゴリズムなんでその辺よろしく^^;\n";
print OUT " */\n";
print OUT "\n";
print OUT "#ifndef __YTL_DELEGATE_IS_COMPATIBLE_FUNCTION_HPP__\n";
print OUT "#define __YTL_DELEGATE_IS_COMPATIBLE_FUNCTION_HPP__\n";
print OUT "\n";
print OUT "#include \"yes_no_type.hpp\"\n";
print OUT "#include \"is_same.hpp\"\n";
print OUT "#include \"is_convertible.hpp\"\n";
print OUT "#include \"select_type.hpp\"\n";
print OUT "\n";
print OUT "namespace yaneuraoGameSDK3rd {\n";
print OUT "namespace YTL {\n";
print OUT "namespace YTLdelegate {\n";
print OUT "namespace is_compatible_function_detail {\n";
print OUT "\n";

print OUT "template<typename From, typename To>\n";
print OUT "struct result_checker {\n";
print OUT "	#ifdef _MSC_VER\n";
print OUT "	enum { value = is_same<void,From>::value || is_convertible<To,From>::value };\n";
print OUT "	#else\n";
print OUT "	static const bool value = is_same<void,From>::value || is_convertible<To,From>::value;\n";
print OUT "	#endif\n";
print OUT "	typedef typename select_type<(result_checker::value), yes_type, no_type>::type type;\n";
print OUT "};\n";
print OUT "template<typename From, typename To>\n";
print OUT "struct arg_checker {\n";
print OUT "	typedef typename select_type<(is_convertible<From,To>::value), yes_type, no_type>::type type;\n";
print OUT "};\n";

$TotalArgs = $ARGV[0];
print OUT "//\t$TotalArgs引数関数までの互換性チェックが可能\\n";

for ($CurrentArgs=0; $CurrentArgs<=$TotalArgs; ++$CurrentArgs) {
	for ($IsToClassMethod=0; $IsToClassMethod<=1; ++$IsToClassMethod) {
		for ($IsFromClassMethod=0; $IsFromClassMethod<=1; ++$IsFromClassMethod) {
			for ($IsToConstClassMethod=0; $IsToConstClassMethod<=$IsToClassMethod; ++$IsToConstClassMethod) {
				for ($IsFromConstClassMethod=0; $IsFromConstClassMethod<=$IsFromClassMethod; ++$IsFromConstClassMethod) {
					#コメント
					print OUT "//\t$CurrentArgs引数: ";
					if ( $IsFromConstClassMethod ) { print OUT "const "; }
					if ( $IsFromClassMethod ) { print OUT "class method"; }
					else { print OUT "static method"; }
					print OUT " → ";
					if ( $IsToConstClassMethod ) { print OUT "const "; }
					if ( $IsToClassMethod ) { print OUT "class method"; }
					else { print OUT "static method"; }
					print OUT "\n";

					#関数宣言
					print OUT "template<";
					if ( $IsFromClassMethod ) { print OUT "class fC,"; }
					if ( $IsToClassMethod ) { print OUT "class tC,"; }
					print OUT "typename fR, typename tR";
					for ($i=1; $i<=$CurrentArgs; ++$i) {
						print OUT ", typename fA$i, typename tA$i";
					}
					print OUT ">\n";
					print OUT "static yes_type test(fR(";
					if ( $IsFromClassMethod ) { print OUT "fC::"; }
					print OUT "*from)(";
					for ($i=1; $i<=$CurrentArgs; ++$i) {
						print OUT "fA$i";
						if ( $i!=$CurrentArgs ) { print OUT ","; }
					}
					print OUT ")";
					if ( $IsFromConstClassMethod ) { print OUT "const"; }
					print OUT ", tR(";
					if ( $IsToClassMethod ) { print OUT "tC::"; }
					print OUT "*to)(";
					for ($i=1; $i<=$CurrentArgs; ++$i) {
						print OUT "tA$i";
						if ( $i!=$CurrentArgs ) { print OUT ","; }
					}
					print OUT ")";
					if ( $IsToConstClassMethod ) { print OUT "const"; }
					print OUT ", typename result_checker<fR,tR>::type*";
					for ($i=1; $i<=$CurrentArgs; ++$i) {
						print OUT ", typename arg_checker<fA$i,tA$i>::type*";
					}
					for ($i=$CurrentArgs+1; $i<=$TotalArgs; ++$i) {
						print OUT ", yes_type*";
					}
					print OUT ");\n";
				}
			}
		}
	}
}

print OUT "\n";
print OUT "static no_type test(...);\n";
print OUT "\n";
print OUT "} // end of namespace is_compatible_function_detail\n";
print OUT "\n";

print OUT "template<class From, typename To>\n";
print OUT "struct is_compatible_function\n";
print OUT "{\n";
print OUT "	#ifdef _MSC_VER\n";
print OUT "\tenum { value = sizeof(yes_type)==sizeof(is_compatible_function_detail::test((From)0,(To)0,(yes_type*)0";
for ($i=1; $i<=$TotalArgs; ++$i) {
	print OUT ",(yes_type*)0";
}
print OUT ")) };\n";
print OUT "	#else\n";
print OUT "\tstatic const bool value = sizeof(yes_type)==sizeof(is_compatible_function_detail::test((From)0,(To)0,(yes_type*)0";
for ($i=1; $i<=$TotalArgs; ++$i) {
	print OUT ",(yes_type*)0";
}
print OUT "));\n";
print OUT "	#endif\n";

print OUT "};\n";

print OUT "\n";
print OUT "} // end of namespace YTLdelegate\n";
print OUT "} // end of namespace YTL\n";
print OUT "} // end of namespace yaneuraoGameSDK3rd\n";
print OUT "\n";
print OUT "#endif // __YTL_DELEGATE_IS_COMPATIBLE_FUNCTION_HPP__\n";

close OUT