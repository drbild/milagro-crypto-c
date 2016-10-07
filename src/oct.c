/**
 * @file oct.c
 * @author Mike Scott
 * @author Kealan McCusker
 * @date 19th May 2015
 * @brief AMCL Basic Octet string maintainance routines
 *
 * @section LICENSE
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/* AMCL Basic Octet string maintainance routines */

/* SU=m, m is Stack Usage */

#include <string.h>
#include "amcl.h"

/* Output an octet string (Debug Only) */

/* SU= 16, formats and outputs an octet to the console in hex */
void OCT_output(octet *w)
{
    int i;
    unsigned char ch;
    for (i=0; i<w->len; i++)
    {
        ch=w->val[i];
        printf("%02x",ch);
    }
    printf("\n");
}

/* SU= 16, formats and outputs an octet to the console as a character string */
void OCT_output_string(octet *w)
{
    int i;
    unsigned char ch;
    for (i=0; i<w->len; i++)
    {
        ch=w->val[i];
        printf("%c",ch);
    }
}

/* Convert C string to octet format - truncates if no room  */
void OCT_jstring(octet *y,char *s)
{
    int i,j;
    i=y->len;
    j=0;
    while (s[j]!=0 && i<y->max)
    {
        y->val[i]=s[j];
        y->len++;
        i++;
        j++;
    }
}

/* SU= 8, compare 2 octet strings. */ 
int OCT_comp(octet *x,octet *y)
{
    int i;
    if (x->len>y->len) return 0;
    if (x->len<y->len) return 0;
    for (i=0; i<x->len; i++)
    {
        if (x->val[i]!=y->val[i]) return 0;
    }
    return 1;
}

/* check are first n bytes the same */
int OCT_ncomp(octet *x,octet *y,int n)
{
    int i;
    if (n>y->len || n>x->len) return 0;
    for (i=0; i<n; i++)
    {
        if (x->val[i]!=y->val[i]) return 0;
    }
    return 1;
}

/* Shift octet to the left by n bytes. Leftmost bytes disappear  */
void OCT_shl(octet *x,int n)
{
    int i;
    if (n>=x->len)
    {
        x->len=0;
        return;
    }
    x->len-=n;
    for (i=0; i<x->len; i++)
        x->val[i]=x->val[i+n];
}

/* SU= 12, append binary string to octet, truncates if no room */
void OCT_jbytes(octet *y,char *b,int len)
{
    int i,j;
    i=y->len;
    for (j=0; j<len && i<y->max; j++)
    {
        y->val[i]=b[j];
        y->len++;
        i++;
    }
}

/* SU= 8, concatenates two octet strings */
void OCT_joctet(octet *y,octet *x)
{
    /* y=y || x */
    int i,j;
    if (x==NULL) return;

    for (i=0; i<x->len; i++)
    {
        j=y->len+i;
        if (j>=y->max)
        {
            y->len=y->max;
            return;
        }
        y->val[j]=x->val[i];
    }
    y->len+=x->len;
}

/* SU= 8, append byte to octet rep times */
void OCT_jbyte(octet *y,int ch,int rep)
{
    int i,j;
    i=y->len;
    for (j=0; j<rep && i<y->max; j++)
    {
        y->val[i]=ch;
        y->len++;
        i++;
    }
}

/* SU= 8, XOR common bytes of a pair of Octets */
void OCT_xor(octet *y,octet *x)
{
    /* xor first x->len bytes of y */

    int i;
    for (i=0; i<x->len && i<y->len; i++)
    {
        y->val[i]^=x->val[i];
    }
}

/* clear an octet */
void OCT_empty(octet *w)
{
    w->len=0;
}

/* Kill an octet string - Zeroise it for security */
void OCT_clear(octet *w)
{
    int i;
    for (i=0; i<w->max; i++) w->val[i]=0;
    w->len=0;
}

/* SU= 8, appends n bytes of integer m to end of Octet O (big endian) */
void OCT_jint(octet *y,int x,int len)
{
    int i,n;
    n=y->len+len;
    if (n>y->max || len<=0) return;
    for (i=y->len; i<n; i++) y->val[i]=0;
    y->len=n;

    i=y->len;
    while (x>0 && i>0)
    {
        i--;
        y->val[i]=x%256;
        x/=256;
    }
}


/* SU= 8, Pad an octet to a given length */
int OCT_pad(octet *w,int n)
{
    int i,d;
    if (w->len>n || n>w->max) return 0;
    if (n==w->len) return 1;
    d=n-w->len;
    for (i=n-1; i>=d; i--)
        w->val[i]=w->val[i-d];
    for (i=d-1; i>=0; i--)
        w->val[i]=0;
    w->len=n;
    return 1;
}

/* SU= 56, Convert an octet string to base64 string */
void OCT_tobase64(char *b,octet *w)
{
    int i,j,k,rem,last;
    int c,ch[4];
    unsigned char ptr[3];
    rem=w->len%3;
    j=k=0;
    last=4;
    while (j<w->len)
    {
        for (i=0; i<3; i++)
        {
            if (j<w->len) ptr[i]=w->val[j++];
            else
            {
                ptr[i]=0;
                last--;
            }
        }
        ch[0]=(ptr[0]>>2)&0x3f;
        ch[1]=((ptr[0]<<4)|(ptr[1]>>4))&0x3f;
        ch[2]=((ptr[1]<<2)|(ptr[2]>>6))&0x3f;
        ch[3]=ptr[2]&0x3f;
        for (i=0; i<last; i++)
        {
            c=ch[i];
            if (c<26) c+=65;
            if (c>=26 && c<52) c+=71;
            if (c>=52 && c<62) c-=4;
            if (c==62) c='+';
            if (c==63) c='/';
            b[k++]=c;
        }
    }
    if (rem>0) for (i=rem; i<3; i++) b[k++]='=';
    b[k]='\0';  /* dangerous! */
}

/* SU= 56, populate an Octet from base64 number */
void OCT_frombase64(octet *w,char *b)
{
    int i,j,k,pads,len=(int)strlen(b);
    int c,ch[4],ptr[3];
    /* int lead=1; */
    j=k=0;
    while (j<len && k<w->max)
    {
        pads=0;
        for (i=0; i<4; i++)
        {
            c=80+b[j++];
            if (c<=112) continue; /* ignore white space */
            if (c>144 && c<171) c-=145;
            if (c>176 && c<203) c-=151;
            if (c>127 && c<138) c-=76;
            if (c==123) c=62;
            if (c==127) c=63;
            if (c==141)
            {
                pads++;    /* ignore pads '=' */
                continue;
            }
            ch[i]=c;
        }
        ptr[0]=(ch[0]<<2)|(ch[1]>>4);
        ptr[1]=(ch[1]<<4)|(ch[2]>>2);
        ptr[2]=(ch[2]<<6)|ch[3];
        for (i=0; i<3-pads && k<w->max; i++)
        {
            /* don't put in leading zeros */
            /* if (lead && ptr[i]==0) continue; */
            w->val[k++]=ptr[i];
            /* lead=0; */
        }

    }
    w->len=k;
}

/* SU= 16, copy an octet string - truncates if no room */
void OCT_copy(octet *y,octet *x)
{
    int i;
    OCT_clear(y);
    y->len=x->len;
    if (y->len>y->max) y->len=y->max;

    for (i=0; i<y->len; i++)
        y->val[i]=x->val[i];
}

/* XOR m with all of x */
void OCT_xorbyte(octet *x,int m)
{
    int i;
    for (i=0; i<x->len; i++) x->val[i]^=m;
}

/* SU= 8, truncates x to n bytes and places the rest in y (if y is not NULL) */
void OCT_chop(octet *x,octet *y,int n)
{
    int i;
    if (n>=x->len)
    {
        if (y!=NULL) y->len=0;
        return;
    }
    if (y!=NULL) y->len=x->len-n;
    x->len=n;

    if (y!=NULL)
    {
        for (i=0; i<y->len && i<y->max; i++) y->val[i]=x->val[i+n];
    }
}

/* Create an Octet from bytes taken from a random number generator */
void OCT_rand(octet *x,csprng *RNG,int len)
{
    int i;
    if (len>x->max) len=x->max;
    x->len=len;

    for (i=0; i<len; i++) x->val[i]=RAND_byte(RNG);
}

/* Convert an Octet to a hex string */
void OCT_toHex(octet *src,char *dst)
{
    int i;
    unsigned char ch;
    for (i=0; i<src->len; i++)
    {
        ch=src->val[i];
        sprintf(&dst[i*2],"%02x", ch);
    }
}

/* Internal function */
static int char2int(char input)
{
    if(input >= '0' && input <= '9')
        return input - '0';
    if(input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if(input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    return 0;
}

/* Convert a hex number to an Octet */
void OCT_fromHex(octet *dst,char *src)
{
    int i=0;
    int j=0;
    OCT_clear(dst);

    while(src[j]!=0)
    {
        dst->val[i++] = char2int(src[j])*16 + char2int(src[j+1]);
        j += 2;
    }
    dst->len=i;
}


/* Convert an Octet to string */
void OCT_toStr(octet *src,char *dst)
{
    int i;
    unsigned char ch;
    for (i=0; i<src->len; i++)
    {
        ch=src->val[i];
        sprintf(&dst[i],"%c", ch);
    }
}

/* Test program
#include <stdio.h>
#include "amcl.h"

char test[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

int main()
{
	char gm[100],gn[100],t[100];
    octet m={0,sizeof(gm),gm};
    octet n={0,sizeof(gn),gn};

	OCT_jbytes(&m,test,strlen(test));
	OCT_output(&m);

	OCT_tobase64(t,&m);
	printf(t); printf("\n");

	OCT_frombase64(&n,t);
	OCT_output(&n);

    return 0;
}
*/
