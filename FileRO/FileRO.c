#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(void)
{

FILE *fi, *fo;

char fr[256]={};
char fw[256]={};
char buf[1014];
printf("\n Input File Name please: \n");
scanf("%256s",fr);
printf("\n Output File Name Please \n");
scanf("%256s",fw);
fi = fopen(fr,"r");
fo = fopen(fr,"w");
if(!fi)
{
printf("\n Cant open input file \n");
return -1;
}
if(!fo)
{
printf("\n Cant open output file\n");
return -1;
}

while (!feof(fi))
{
memset(buf,0,1024);
size_t s = fread(buf,1,1024,fi);
fwrite(buf,1,s,fo);
}


fclose(fi);
fclose(fo);
return 0;
}


