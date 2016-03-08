#bin/perl

use strict;
use warnings;
use File::Find;

find(\&wanted,"./caplNodes");

sub wanted
{
    if(-f $_ and $_ =~ m/.*?\.can/)
    {
        edit_file($_);
    }
}

sub edit_file
{
    my ($filename) = @_;

    local $/ =undef;
    open(FILE, $filename) || die "File not found";
    binmode FILE;
    my $fileStr = <FILE>;
    close(FILE);
    
    for($fileStr){
        s/variables.*?\[.*?\]/hello/g;
        s/on start/void onstart()/g;
        s/on message/void onmessage()/g;
    }

    $filename =~ s/\.can/\.cpp/g;
    $filename = "../cppNodes/".$filename;
    my $outfile = $filename;
    open(FILE, ">$outfile") || die "File not found";
    print FILE $fileStr;
    close(FILE);

}

