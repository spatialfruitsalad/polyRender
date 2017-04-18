#!/usr/bin/awk -f

## small tool to calculate mean and std for a 9 column File


BEGIN{
        N = 0
        Sum1 = 0
        Sum2 = 0
        Sum3 = 0
        Sum4 = 0
        Sum5 = 0
        Sum6 = 0
        Sum7 = 0
        Sum8 = 0
        Sum9 = 0
}

$1 !~/^\#/{
 if(length($1) != 0 && $1 != "NaN")
{
  N = N+1
  Sum1 += $1
  arr1[N] = $1
  Sum2 += $2
  arr2[N] = $2
  Sum3 += $3
  arr3[N] = $3
  Sum4 += $4
  arr4[N] = $4
  Sum5 += $5
  arr5[N] = $5
  Sum6 += $6
  arr6[N] = $6
  Sum7 += $7
  arr7[N] = $7
  Sum8 += $8
  arr8[N] = $8
  Sum9 += $9
  arr9[N] = $9
}
}
END {
     print "## Mean std " 
     print "# read lines: " N 
     mean1 = Sum1/N; 
     std1=0.0;
     mean2 = Sum2/N; 
     std2=0.0;
     mean3 = Sum3/N; 
     std3=0.0;
     mean4 = Sum4/N; 
     std4=0.0;
     mean5 = Sum5/N; 
     std5=0.0;
     mean6 = Sum6/N; 
     std6=0.0;
     mean7 = Sum7/N; 
     std7=0.0;
     mean8 = Sum8/N; 
     std8=0.0;
     mean9 = Sum9/N; 
     std9=0.0;
     for (i = 1; i<=N; i++) {
             std1+=(mean1-arr1[i])**2;
             std2+=(mean2-arr2[i])**2;
             std3+=(mean3-arr3[i])**2;
             std4+=(mean4-arr4[i])**2;
             std5+=(mean5-arr5[i])**2;
             std6+=(mean6-arr6[i])**2;
             std7+=(mean7-arr7[i])**2;
             std8+=(mean8-arr8[i])**2;
             std9+=(mean9-arr9[i])**2;
     } 
      std1=std1/(N-1);
      std1=sqrt(std1);
if(mean1 != 0)
{
      printf("#label# %1.8g %1.8e %3.3f% \n", mean1, std1, std1/mean1*100);
}
else
{
      printf("## %1.8e %1.8e \n", mean1, std1);
}
      std2=std2/(N-1);
      std2=sqrt(std2);
if(mean2 != 0)
{
      printf("#cx# %1.8g %1.8e %3.3f% \n", mean2, std2, std2/mean2*100);
}
else
{
      printf("## %1.8e %1.8e \n", mean2, std2);
}
      std3=std3/(N-1);
      std3=sqrt(std3);
if(mean3 != 0)
{
      printf("#cy# %1.8g %1.8e %3.3f% \n", mean3, std3, std3/mean3*100);
}
else
{
      printf("## %1.8e %1.8e \n", mean3, std3);
}
      std4=std4/(N-1);
      std4=sqrt(std4);
if(mean4 != 0)
{
      printf("#cz# %1.8g %1.8e %3.3f% \n", mean4, std4, std4/mean4*100);
}
else
{
      printf("## %1.8e %1.8e \n", mean4, std4);
}
      std5=std5/(N-1);
      std5=sqrt(std5);
if(mean5 != 0)
{
      printf("## %1.8e %1.8e %3.3f% \n", mean5, std5, std5/mean5*100);
}
else
{
      printf("## %1.8e %1.8e \n", mean5, std5);
}
      std6=std6/(N-1);
      std6=sqrt(std6);
if(mean6 != 0)
{
      printf("## %1.8e %1.8e %3.3f% \n", mean6, std6, std6/mean6*100);
}
else
{
      printf("## %1.8e %1.8e \n", mean6, std6);
}
      std7=std7/(N-1);
      std7=sqrt(std7);
if(mean7 != 0)
{
      printf("## %1.8e %1.8e %3.3f% \n", mean7, std7, std7/mean7*100);
}
else
{
      printf("## %1.8e %1.8e \n", mean7, std7);
}
      std8=std8/(N-1);
      std8=sqrt(std8);
if(mean8 != 0)
{
      printf("## %1.8e %1.8e %3.3f% \n", mean8, std8, std8/mean8*100);
}
else
{
      printf("## %1.8e %1.8e \n", mean8, std8);
}
      std9=std9/(N-1);
      std9=sqrt(std9);
if(mean9 != 0)
{
      printf("## %1.8e %1.8e %3.3f% \n", mean9, std9, std9/mean9*100);
}
else
{
      printf("## %1.8e %1.8e \n", mean9, std9);
}
}
