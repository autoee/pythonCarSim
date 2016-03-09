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

sub removeSpaces
{
    my ($word) = @_;
    $word =~ s/\s//g;
    return $word;
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
        #convert callbacks to functions
        s/on (.*?)\n/"void on".removeSpaces($1)."()"/ge;
        #convert functions to c function
        s/\n([a-zA-Z_]+\(.*?\)\s*\n)/void $1/g;
        #remove variable block
        s/variables\s*\n{((?>[^}{]+|(?R))*)}/$1/gs;
        #remove includes block
        s/includes\s*\n{((?>[^}{]+|(?R))*)}/$1/gs;
    }

    $filename =~ s/\.can/\.cpp/g;
    $filename = "../cppNodes/".$filename;
    my $outfile = $filename;
    open(FILE, ">$outfile") || die "File not found";
    print FILE $fileStr;
    close(FILE);

}

