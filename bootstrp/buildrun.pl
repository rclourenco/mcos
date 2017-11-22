
use File::Basename;

my $dir = dirname($0);

#system("dosbox -c m:\\buildx.bat");
system( "cp ".quotemeta("$dir/../NEW.IMG")." ".quotemeta("$dir/mcos.img") );
system( "bochs -f ".quotemeta("$dir/bf.cfg") );

