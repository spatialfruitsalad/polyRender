#!/usr/bin/awk -f

## small tool to calculate min and max for a 4 column File

BEGIN{
        Min1 =  100000000
        Min2 =  100000000
        Min3 =  100000000
        Min4 =  100000000
        Max1 = -100000000
        Max2 = -100000000
        Max3 = -100000000
        Max4 = -100000000
}

$1 !~/^\#/{
 if(length($1) != 0 && $1 != "NaN")
{
    if($1 < Min1) Min1 =$1
    if($2 < Min2) Min2 =$2
    if($3 < Min3) Min3 =$3
    if($4 < Min4) Min4 =$4

    if($1 > Max1) Max1 =$1
    if($2 > Max2) Max2 =$2
    if($3 > Max3) Max3 =$3
    if($4 > Max4) Max4 =$4
}
}
END {
     print "## Min Max"
     printf("#label# %1.8g %1.8g \n",  Min1, Max1);
     printf("#cx# %1.8g %1.8g \n",  Min2, Max2);
     printf("#cy# %1.8g %1.8g \n",  Min3, Max3);
     printf("#cz# %1.8g %1.8g \n",  Min4, Max4);
}
