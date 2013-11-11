@rem = '--*-Perl-*--
@echo off
perl -x -S %0 %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
if errorlevel 1 goto script_failed_so_exit_with_non_zero_val 2>nul
goto endofperl
@rem ';
#!/usr/bin/perl -w

use strict;

my $tot_version_file = '.\WebKit\s60\tot_version.txt';
my $headfilename = 'head.txt';
my $head_r = 'HEAD';

print "Getting svn HEAD info off http://svn.webkit.org/repository/webkit/branches/Safari-3-branch\n";
system "svn info -r HEAD http://svn.webkit.org/repository/webkit/branches/Safari-3-branch > $headfilename";
open( FILE, "+< $headfilename" ) or die "Can't open $headfilename : $!";
my @svn = <FILE>;

foreach my $line (@svn) {
        if($line =~ m/Revision: (.*?)\n/) {
                $head_r = $1;
                last;
        }
}


close (FILE);
unlink($headfilename) or warn "cannot delete $headfilename";

open( FILE, "< $tot_version_file" ) or die "Can't open $tot_version_file : $!";
my @data = <FILE>;

foreach my $line (@data) {
        chomp $line;
        print "Start diffing $line:$head_r\n";
        system "svn diff -r $line:$head_r http://svn.webkit.org/repository/webkit/branches/Safari-3-branch >> webkit$head_r.txt";
}
close (FILE);
print "diff is in webkit$head_r.txt\n";
print "modify it as you wish...and press Enter to continue.\n";
<STDIN>;
print "Start applying webkit$head_r.txt to .\n\n\n";
system ".\\WebkitTools\\S60Tools\\svn-apply webkit$head_r.txt";
print "\n\n\nUpdate $tot_version_file with $head_r, please!\n";

__END__
:endofperl
