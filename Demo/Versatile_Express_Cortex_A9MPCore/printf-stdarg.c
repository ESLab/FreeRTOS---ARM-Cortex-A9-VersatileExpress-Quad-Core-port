/*
	Copyright 2001, 2002 Georges Menie (www.menie.org)
	stdarg version contributed by Christian Ettinger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdarg.h>
#include <stdbool.h>

#ifdef PRINTF_FLOAT_SUPPORT
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#endif

static int print(char **out,const char *format,va_list args);
static char *formatinteger(char *bufferend,unsigned int absvalue,
unsigned int base,char letterbase,int zeropadwidth,bool zeroisempty);
#ifdef PRINTF_FLOAT_SUPPORT
static char *formatfloat(char *bufferstart,double absvalue,
int precision,int zeropadwidth,bool forceperiod);
#endif
static int printstring(char **out,const char *string,int width,bool padleft);
static void printchar(char **out,int c);
int putchar(int c);

int printf(const char *format,...)
{
	va_list args;
	va_start(args,format);
	return print(0,format,args);
}

int sprintf(char *out,const char *format,...)
{
	va_list args;
	va_start(args,format);
	return print(&out,format,args);
}

int puts(const char *s)
{
	return printstring(0,s,0,false);
	//printf("%s",s);
}

static int print(char **out,const char *format,va_list args)
{
	char buffer[128];
	char *bufferend=&buffer[sizeof(buffer)];
	int count=0;

	while(*format)
	{
		if(*format=='%')
		{
			format++;

			if(*format=='%') goto printnormal;

			int width=0,precision=-1;
			char positivechar=0;
			bool padleft=true,padzero=false,alternate=false;

			// Parse flag characters.
			for(;;)
			{
				if(*format==0)
				{
					// Premature format string end.
					goto end;
				}
				else if(*format=='#')
				{
					// Alternate form.
					alternate=true;
					format++;
				}
				else if(*format=='-')
				{
					// Left justify. Overrides zero padding.
					padleft=false;
					padzero=false;
					format++;
				}
				else if(*format=='0')
				{
					// Zero padding.
					// Overriden by left justification, so check for that.
					if(padleft) padzero=true;
					format++;
				}
				else if(*format=='+')
				{
					// Prefix positive numbers with a plus sign. Overrides space prefix.
					positivechar='+';
					format++;
				}
				else if(*format==' ')
				{
					// Prefix positive numbers with a space.
					// Overriden by plus prefix, so check for that.
					if(!positivechar) positivechar=' ';
					format++;
				}
				else
				{
					break;
				}
			}

			// Parse field width, if any.
			while(*format>='0'&&*format<='9')
			{
				width*=10;
				width+=*format-'0';
				format++;
			}

			// Check for and parse precision.
			if(*format=='.')
			{
				format++;

				precision=0;
				while(*format>='0'&&*format<='9')
				{
					precision*=10;
					precision+=*format-'0';
					format++;
				}
			}

			// Ignore length modifiers. (Only "l" is handled for now.)
			if(*format=='l') format++;

			// Parse the conversion specifier and print appropriate output.
			if(*format=='s')
			{
				char *string=va_arg(args,char *);
				count+=printstring(out,string?string:"(null)",width,padleft);
			}
			else if(*format=='d' || *format=='i')
			{
				int value=va_arg(args,int);

				// Check if the number is signed and negative.
				unsigned int absvalue=value;
				bool isnegative=false;
				if(value<0)
				{
					isnegative=true;
					absvalue=-value;
				}

				// If zero padding was requested and no precision was specified,
				// set the precision to fill the available space with zeroes.
				int zeropadwidth=precision;
				if(padzero && precision<0)
				{
					zeropadwidth=width;
					if(isnegative || positivechar) zeropadwidth-=1; // Reserve space for a sign, if needed.
				}

				bool zeroisempty=precision==0;

				char *string=formatinteger(bufferend,absvalue,10,0,zeropadwidth,zeroisempty);

				// Handle the sign.
				if(isnegative) *--string='-';
				else if(positivechar) *--string=positivechar;

				count+=printstring(out,string,width,padleft);
			}
			else if(*format=='u')
			{
				int value=va_arg(args,int);

				int zeropadwidth=precision;
				if(padzero && precision<0) zeropadwidth=width;

				bool zeroisempty=precision==0;

				char *string=formatinteger(bufferend,value,10,0,zeropadwidth,zeroisempty);
				count+=printstring(out,string,width,padleft);
			}
			else if(*format=='x' || *format=='X')
			{
				int value=va_arg(args,int);

				int zeropadwidth=precision;
				if(padzero && precision<0)
				{
					zeropadwidth=width;
					if(alternate && value!=0) zeropadwidth-=2;
				}

				bool zeroisempty=precision==0;

				char *string=formatinteger(bufferend,value,16,'A'-'X'+*format,zeropadwidth,zeroisempty);
				if(alternate && value!=0) { *--string=*format; *--string='0'; }
				count+=printstring(out,string,width,padleft);
			}
			else if(*format=='o')
			{
				int value=va_arg(args,int);

				int zeropadwidth=precision;
				if(padzero && precision<0) zeropadwidth=width;
				if(alternate && value!=0) zeropadwidth-=1;

				bool zeroisempty=precision==0 && !alternate;

				char *string=formatinteger(bufferend,value,8,0,zeropadwidth,zeroisempty);
				if(alternate && value!=0) *--string='0';
				count+=printstring(out,string,width,padleft);
			}
			else if(*format=='c')
			{
				char str[2];
				str[0]=va_arg( args, int );
				str[1]=0;
				count+=printstring(out,str,width,padleft);
			}
			#ifdef PRINTF_FLOAT_SUPPORT
			else if(*format=='f' || *format=='F')
			{
				double value=va_arg(args,double);

				// Check if the number is signed and negative.
				double absvalue=value;
				bool isnegative=false;
				if(copysign(1,value)<0)
				{
					isnegative=true;
					absvalue=-value;
				}

				// Default precision is 6.
				if(precision<0) precision=6;

				// Calculate zero padding.
				int zeropadwidth=0;
				if(padzero)
				{
					zeropadwidth=width;
					// Reserve space for a sign, if needed.
					if(isnegative || positivechar) zeropadwidth-=1;
				}

				char *string=formatfloat(buffer,absvalue,precision,zeropadwidth,alternate);

				// Handle the sign.
				if(isnegative) *--string='-';
				else if(positivechar) *--string=positivechar;

				// Uppercase string if requested.
				if(*format=='F')
				{
					char *ptr=string;
					while(*ptr) { *ptr=toupper(*ptr); ptr++; }
				}

				count+=printstring(out,string,width,padleft);
			}
			#endif
			else
			{
				// Invalid format or premature format string end, fall through.
			}
		}
		else
		{
			printnormal:
			printchar(out,*format);
			count++;
		}

		format++;
	}

	end: (void)0;

	if(out) **out=0;

	va_end(args);

	return count;
}

static char *formatinteger(char *bufferend,unsigned int absvalue,
unsigned int base,char letterbase,int zeropadwidth,bool zeroisempty)
{
	char *string=bufferend;
	*--string=0;

	// Generate the digits in reverse.
	if(!absvalue)
	{
		if(!zeroisempty) *--string='0';
	}
	else
	{
		while(absvalue)
		{
			unsigned int digit=absvalue%base;

			char c;
			if(digit>=10) c=digit-10+letterbase;
			else c=digit+'0';

			*--string=c;

			absvalue/=base;
		}
	}

	// Zero pad the number if requested.
	if(zeropadwidth>0)
	{
		int length=bufferend-string-1;
		for(int i=0;i<zeropadwidth-length;i++) *--string='0';
	}

	return string;
}

#ifdef PRINTF_FLOAT_SUPPORT

static char *formatfloat(char *bufferstart,double absvalue,int precision,int zeropadwidth,bool forceperiod)
{
	absvalue+=0.5/pow(10,precision);

	union { uint64_t l; double f; } pun;

	pun.f=absvalue;
	int exp2=((pun.l>>52)&0x7ff)-1023;
	uint64_t mant=pun.l&0x000fffffffffffffULL;

	//if((*sign=x.l>>63)) value=-value;

	if(exp2==0x400)
	{
		if(mant) strcpy(bufferstart,"nan");
		else strcpy(bufferstart,"inf");
		return bufferstart;
	}

	// Find base-10 exponent.
//	int exp10=(absvalue==0)?!fflag:(int)ceil(log10(absvalue));
	int exp10=(int)ceil(log10(absvalue));
	if(exp10<-307) exp10=-307; // Avoid overflow in pow()

	// Attempt to scale value to 0.1<=x<1.0.
	absvalue*=pow(10.0,-exp10);

	// Correct scaling if it went wrong.
	if(absvalue)
	{
		while(absvalue<0.1) { absvalue*=10; exp10--; }
		while(absvalue>=1.0) { absvalue/=10; exp10++; }
	}

	// Calculate the numbers and positions to print.
	int numberofdigits,firstdigit,decimalpoint;
	if(exp10>0)
	{
		numberofdigits=exp10+precision+1;
		firstdigit=0;
		decimalpoint=exp10;
	}
	else
	{
		numberofdigits=precision+2;
		firstdigit=-exp10+2;
		decimalpoint=1;

	}

	// Elide decimal point for 0-precision, except when explicitly requested.
	if(precision==0)
	{
		if(!forceperiod) numberofdigits--;
	}

	pun.f=absvalue;
	exp2=((pun.l>>52)&0x7ff)-1023;
	mant=pun.l&0x000fffffffffffffULL;

	if(exp2==-1023) exp2++;
	else mant|=0x0010000000000000ULL;

	// TODO: What is this supposed to do?
	mant<<=(exp2+4); // 56-bit denormalised signifier

	char *ptr=bufferstart;

	// Zero pad if requested.
	if(numberofdigits<zeropadwidth)
	{
		int pad=zeropadwidth-numberofdigits;
		for(int i=0;i<pad;i++) *ptr++='0';
	}

	for(int i=0;i<numberofdigits;i++)
	{
		if(i==decimalpoint)
		{
			*ptr++='.';
		}
		else if(i<firstdigit)
		{
			*ptr++='0';
		}
		else
		{
			mant&=0x00ffffffffffffffULL; // mod 1.0
			mant*=10;
			*ptr++=(mant>>56)+'0';
		}
	}

	*ptr++=0;
	return bufferstart;
}
#endif

static int printstring(char **out,const char *string,int width,bool padleft)
{
	int count=0;

	// If we need to pad on the left, calculate the length of the string and
	// print the appropriate amount of padding.
	if(width>0 && padleft)
	{
		const char *end=string;
		while(*end) end++;

		int length=end-string;
		for(int i=0;i<width-length;i++)
		{
			printchar(out,' ');
			count++;
		}
	}

	// Print the contents of the string.
	while(*string)
	{
		printchar(out,*string);
		count++;
		string++;
	}

	// Print any remaining padding on the right, if needed.
	while(count<width)
	{
		printchar(out,' ');
		count++;
	}

	return count;
}

static void printchar(char **out,int c)
{
	if(out) *(*out)++=c;
	else putchar(c);
}




#ifndef TEST_PRINTF
extern void xUARTSendCharacter(unsigned long ulUARTPeripheral,signed char cChar,unsigned long xDelay);

static inline unsigned long portCORE_ID(void)
{
	unsigned long val;
	__asm(" mrc p15,0,%[val],c0,c0,5\n":[val] "=r" (val)::);
	return val&3;
}

int putchar(int c)
{
	xUARTSendCharacter(portCORE_ID(),c,0);
	return 0;
}
#endif



#ifdef TEST_PRINTF
int main(void)
{
	char *ptr = "Hello world!";
	char *np = 0;
	int i = 5;
	unsigned int bs = sizeof(int)*8;
	int mi;
	char buf[80];

	mi = (1 << (bs-1)) + 1;
	printf("%s\n", ptr);
	printf("printf test\n");
	printf("%s is null pointer\n", np);
	printf("%d = 5\n", i);
	printf("%d = - max int\n", mi);
	printf("char %c = 'a'\n", 'a');
	printf("hex %x = ff\n", 0xff);
	printf("hex %02x = 00\n", 0);
	printf("signed %d = unsigned %u = hex %x\n", -3, -3, -3);
	printf("%d %s(s)%", 0, "message");
	printf("\n");
	printf("%d %s(s) with %%\n", 0, "message");
	sprintf(buf, "justif: \"%-10s\"\n", "left"); printf("%s", buf);
	sprintf(buf, "justif: \"%10s\"\n", "right"); printf("%s", buf);
	sprintf(buf, " 3: %04d zero padded\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %-4d left justification\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %4d right justification\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %-04d zero padded with left justification\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %.4d with precision\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %-.4d with precision and left justification\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %4.2d with precision and width\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %-4.2d with precision, width and left justification\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %04.2d with precision, width and zero padding\n", 3); printf("%s", buf);
	sprintf(buf, " 3: % d with space sign\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %+d with plus sign\n", 3); printf("%s", buf);
	sprintf(buf, " 3: % 4d padded with space sign\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %+4d padded with plus sign\n", 3); printf("%s", buf);
	sprintf(buf, " 3: % 04d zero padded with space sign\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %+04d zero padded with plus sign\n", 3); printf("%s", buf);
	sprintf(buf, "-3: %04d zero padded\n", -3); printf("%s", buf);
	sprintf(buf, "-3: %-4d left justification\n", -3); printf("%s", buf);
	sprintf(buf, "-3: %4d right justification\n", -3); printf("%s", buf);
	sprintf(buf, "-3: %-04d zero padded with left justification\n", -3); printf("%s", buf);
	sprintf(buf, "-3: %.4d with precision\n", -3); printf("%s", buf);
	sprintf(buf, "-3: %-.4d with precision and left justification\n", -3); printf("%s", buf);
	sprintf(buf, "-3: %4.2d with precision and width\n", -3); printf("%s", buf);
	sprintf(buf, "-3: %-4.2d with precision, width and left justification\n", -3); printf("%s", buf);
	sprintf(buf, "-3: %04.2d with precision, width and zero padding\n", -3); printf("%s", buf);
	#ifdef PRINTF_FLOAT_SUPPORT
	sprintf(buf, "3.141592 precision 0: %.0f\n", 3.141592); printf("%s", buf);
	sprintf(buf, "3.141592 precision 1: %.1f\n", 3.141592); printf("%s", buf);
	sprintf(buf, "3.141592 precision 2: %.2f\n", 3.141592); printf("%s", buf);
	sprintf(buf, "3.141592 precision 3: %.3f\n", 3.141592); printf("%s", buf);
	sprintf(buf, "3.141592 precision 4: %.4f\n", 3.141592); printf("%s", buf);
	sprintf(buf, "3.141592 precision 5: %.5f\n", 3.141592); printf("%s", buf);
	sprintf(buf, "3.141592 precision 6: %.6f\n", 3.141592); printf("%s", buf);
	#endif

	return 0;
}

/*
 * if you compile this file with
 *   gcc -Wall $(YOUR_C_OPTIONS) -DTEST_PRINTF -c printf.c
 * you will get a normal warning:
 *   printf.c:214: warning: spurious trailing `%' in format
 * this line is testing an invalid % at the end of the format string.
 *
 * this should display (on 32bit int machine) :
 *
 * Hello world!
 * printf test
 * (null) is null pointer
 * 5 = 5
 * -2147483647 = - max int
 * char a = 'a'
 * hex ff = ff
 * hex 00 = 00
 * signed -3 = unsigned 4294967293 = hex fffffffd
 * 0 message(s)
 * 0 message(s) with %
 * justif: "left      "
 * justif: "     right"
 *  3: 0003 zero padded
 *  3: 3    left justification
 *  3:    3 right justification
 *  3: 3    zero padded with left justification
 *  3: 0003 with precision
 *  3: 0003 with precision and left justification
 *  3:   03 with precision and width
 *  3: 03   with precision, width and left justification
 *  3:   03 with precision, width and zero padding
 *  3:  3 with space sign
 *  3: +3 with plus sign
 *  3:    3 padded with space sign
 *  3:   +3 padded with plus sign
 *  3:  003 zero padded with space sign
 *  3: +003 zero padded with plus sign
 * -3: -003 zero padded
 * -3: -3   left justification
 * -3:   -3 right justification
 * -3: -3   zero padded with left justification
 * -3: -0003 with precision
 * -3: -0003 with precision and left justification
 * -3:  -03 with precision and width
 * -3: -03  with precision, width and left justification
 * -3:  -03 with precision, width and zero padding
 */

#endif

