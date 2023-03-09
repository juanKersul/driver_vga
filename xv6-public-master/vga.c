#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "vga.h"
#define	_vmemwr(DS,DO,S,N)	memcpy((char *)P2V((DS) * 16 + (DO)), S, N)

void 
vmemwr(unsigned dst_off, uchar *src, unsigned count)
{
	_vmemwr(get_fb_seg(), dst_off, src, count);
}

void 
set_plane(unsigned p)
{
	unsigned char pmask;

	p &= 3;
	pmask = 1 << p;
/* set read plane */
	outb(VGA_GC_INDEX, 4);
	outb(VGA_GC_DATA, p);
/* set write plane */
	outb(VGA_SEQ_INDEX, 2);
	outb(VGA_SEQ_DATA, pmask);
}

unsigned 
get_fb_seg(void)
{
	unsigned seg;

	outb(VGA_GC_INDEX, 6);
	seg = inb(VGA_GC_DATA);
	seg >>= 2;
	seg &= 3;
	switch(seg)
	{
	case 0:
	case 1:
		seg = 0xA000;
		break;
	case 2:
		seg = 0xB000;
		break;
	case 3:
		seg = 0xB800;
		break;
	}
	return seg;
}

void 
write_font(uchar *buf, unsigned font_height)
{
	uchar seq2, seq4, gc4, gc5, gc6;
	unsigned i;

/* save registers
set_plane() modifies GC 4 and SEQ 2, so save them as well */
	outb(VGA_SEQ_INDEX, 2);
	seq2 = inb(VGA_SEQ_DATA);

	outb(VGA_SEQ_INDEX, 4);
	seq4 = inb(VGA_SEQ_DATA);
/* turn off even-odd addressing (set flat addressing)
assume: chain-4 addressing already off */
	outb(VGA_SEQ_DATA, seq4 | 0x04);

	outb(VGA_GC_INDEX, 4);
	gc4 = inb(VGA_GC_DATA);

	outb(VGA_GC_INDEX, 5);
	gc5 = inb(VGA_GC_DATA);
/* turn off even-odd addressing */
	outb(VGA_GC_DATA, gc5 & ~0x10);

	outb(VGA_GC_INDEX, 6);
	gc6 = inb(VGA_GC_DATA);
/* turn off even-odd addressing */
	outb(VGA_GC_DATA, gc6 & ~0x02);
/* write font to plane P4 */
	set_plane(2);
/* write font 0 */
	for(i = 0; i < 256; i++)
	{
		vmemwr(16384u * 0 + i * 32, buf, font_height);
		buf += font_height;
	}
/* restore registers */
	outb(VGA_SEQ_INDEX, 2);
	outb(VGA_SEQ_DATA, seq2);
	outb(VGA_SEQ_INDEX, 4);
	outb(VGA_SEQ_DATA, seq4);
	outb(VGA_GC_INDEX, 4);
	outb(VGA_GC_DATA, gc4);
	outb(VGA_GC_INDEX, 5);
	outb(VGA_GC_DATA, gc5);
	outb(VGA_GC_INDEX, 6);
	outb(VGA_GC_DATA, gc6);
}

void 
write_regs(uchar *regs)
{
	unsigned i;

/* write MISCELLANEOUS reg */
	outb(VGA_MISC_WRITE, *regs);
	regs++;
/* write SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		outb(VGA_SEQ_INDEX, i);
		outb(VGA_SEQ_DATA, *regs);
		regs++;
	}
/* unlock CRTC registers */
	outb(VGA_CRTC_INDEX, 0x03);
	outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) | 0x80);
	outb(VGA_CRTC_INDEX, 0x11);
	outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) & ~0x80);
/* make sure they remain unlocked */
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;
/* write CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		outb(VGA_CRTC_INDEX, i);
		outb(VGA_CRTC_DATA, *regs);
		regs++;
	}
/* write GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++)
	{
		outb(VGA_GC_INDEX, i);
		outb(VGA_GC_DATA, *regs);
		regs++;
	}
/* write ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++)
	{
		(void)inb(VGA_INSTAT_READ);
		outb(VGA_AC_INDEX, i);
		outb(VGA_AC_WRITE, *regs);
		regs++;
	}
/* lock 16-color palette and unblank display */
	(void)inb(VGA_INSTAT_READ);
	outb(VGA_AC_INDEX, 0x20);
}

/*
	Escribe los registros seteando los colores.
*/
void 
vgasetpalette(int index, int r, int g, int b) 
{
	outb(0x3C8, index);
	outb(0x3C9, r); 
	outb(0x3C9, g); 
	outb(0x3C9, b); 
}

/*
	Settea la paleta de 256 colores.
*/
void 
setpalette()
{
	for(int index=0;index<256;index++) 
	{
    	int value = vga256_24bit[index];
    	vgasetpalette(index,(value>>18)&0x3f,(value>>10)&0x3f,(value>>2)&0x3f);
	}  
}

/*
	Pinta la pantalla, y settea la paleta de 256.
*/
void 
window_background()
{
	uchar *VGA;
	setpalette();
	if (current_mode == VGA_GRAPHIC_MODE)
	{
		VGA = (uchar * ) P2V(0xA0000);
		for (uint i = 0; i < 320*200; i++)
		{
			VGA[i] = 15;	/* Color blanco de la paleta de 256 = 0x0F*/
		}
	} else if (current_mode == VGA_TEXT_MODE)
	{
		VGA = (uchar *)P2V(0xB8000);
		for (uint i = 0; i < 80*25*2; i++)
		{
			VGA[i] = 0; // 0x0
		}
	}
}

/*
	Sysyem call para cambiar de modo texto a modo gráfico,
	sólo hace el cambio si los valores ingresados son 1 o 0.
	No cambia si el valor ingresado es igual al valor actual.
*/
void 
modeswitch(unsigned int mode)
{
    /*
        No cambia si seleccionamos el modo en el que estamos.
    */
    if (current_mode != mode)
	{
        if(mode == VGA_TEXT_MODE)
		{
			write_regs(text);
			write_font(font, 16);
			current_mode = VGA_TEXT_MODE;
			window_background();
			vgainit();
        } else if(mode == VGA_GRAPHIC_MODE)
		{
            write_regs(graphic);
            current_mode = VGA_GRAPHIC_MODE;
            window_background();
        } else {
        	cprintf("Parametro invalido\n");
        }
    }else 
	{
    	cprintf("Ya se encuentra en el modo requerido\n");
    }
}

/*
	Pinta un pixel.
	PRE: Estar en modo gráfico.
*/
void 
plotpixel(int x, int y, int color)
{
	if (current_mode == VGA_GRAPHIC_MODE)
	{
		uchar *VGA = (uchar *)P2V(0xA0000);
		VGA[x + y*320] = color;
	}else
	{
		cprintf("Plotpixel no es compatible con el modo actual\n");
	}
}

/*
	Pinta un rectángulo, hace uso del procedimiento plotpixel.
	PRE: Estar en modo gráfico.
*/
void 
plotrectangle(int x1, int y1, int x2, int y2, int color)
{
	if( current_mode == VGA_GRAPHIC_MODE)
	{
		if (x2>320 || y2>200 || x1 > 320 || y1 > 200){
			cprintf("Parametros invalidos\n");
			exit();
		}else{	
			int x_init = x1;
			int x_end = x2;
			int y_init = y1;
			int y_end = y2;
			int aux;
			if (x_init>x_end)
			{
				aux = x_init;
				x_init = x_end;
				x_end = aux;
			}
			if (y_init>y_end)
			{
				aux = y_init;
				y_init = y_end;
				y_end = aux;
			}
			for(int y = y_init; y < y_end; y++)
			{
				for (int x = x_init; x < x_end; x++)
				{
					plotpixel( x, y, color);
				}
			}
		}
	}else
	{
		cprintf("Plotrectangle no es compatible con el modo actual\n");
	}
}

/*
	Escribe en pantalla "SO2021" y setea en la variable current_mode el valor del textmode.
*/
void 
vgainit(void)
{
	*(int *)P2V(0xB8F94) = 0x4353;
	*(int *)P2V(0xB8F96) = 0x434F;
	*(int *)P2V(0xB8F98) = 0x4332;
	*(int *)P2V(0xB8F9A) = 0x4330;
	*(int *)P2V(0xB8F9C) = 0x4332;
	*(int *)P2V(0xB8F9E) = 0x4331;
    current_mode = VGA_TEXT_MODE;
}
