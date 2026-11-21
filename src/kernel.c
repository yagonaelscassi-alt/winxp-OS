/* WinXP-OS: escritorio estilo Windows XP sobre framebuffer lineal.
 * C freestanding (gnu99), compatible con GCC 6.3 / binutils 2.28 de Debian 9. */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define MULTIBOOT_MAGIC 0x2BADB002

struct mb_info {
    u32 flags;
    u32 mem_lower, mem_upper;
    u32 boot_device, cmdline;
    u32 mods_count, mods_addr;
    u32 syms[4];
    u32 mmap_length, mmap_addr;
    u32 drives_length, drives_addr;
    u32 config_table, boot_loader_name;
    u32 apm_table;
    u32 vbe_control_info, vbe_mode_info;
    u16 vbe_mode, vbe_interface_seg, vbe_interface_off, vbe_interface_len;
    u32 fb_addr_lo, fb_addr_hi;
    u32 fb_pitch, fb_width, fb_height;
    u8  fb_bpp, fb_type;
} __attribute__((packed));

static u8 *fb;
static u32 pitch, W, H;

/* Fuente 8x8 (ASCII 0x20-0x5F, solo mayusculas), bit0 = pixel izquierdo */
static const u8 font[64][8] = {
{0,0,0,0,0,0,0,0},{0x18,0x3C,0x3C,0x18,0x18,0,0x18,0},{0x36,0x36,0,0,0,0,0,0},
{0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0},{0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0},
{0,0x63,0x33,0x18,0x0C,0x66,0x63,0},{0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0},
{0x06,0x06,0x03,0,0,0,0,0},{0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0},
{0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0},{0,0x66,0x3C,0xFF,0x3C,0x66,0,0},
{0,0x0C,0x0C,0x3F,0x0C,0x0C,0,0},{0,0,0,0,0,0x0C,0x0C,0x06},
{0,0,0,0x3F,0,0,0,0},{0,0,0,0,0,0x0C,0x0C,0},{0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0},
{0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0},{0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0},
{0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0},{0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0},
{0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0},{0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0},
{0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0},{0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0},
{0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0},{0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0},
{0,0x0C,0x0C,0,0,0x0C,0x0C,0},{0,0x0C,0x0C,0,0,0x0C,0x0C,0x06},
{0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0},{0,0,0x3F,0,0,0x3F,0,0},
{0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0},{0x1E,0x33,0x30,0x18,0x0C,0,0x0C,0},
{0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0},
{0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0},{0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0},
{0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0},{0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0},
{0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0},{0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0},
{0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0},{0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0},
{0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0},{0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0},
{0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0},{0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0},
{0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0},{0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0},
{0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0},{0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0},
{0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0},{0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0},
{0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0},{0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0},
{0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0},{0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0},
{0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0},{0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0},
{0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0},{0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0},
{0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0},{0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0},
{0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0},{0x08,0x1C,0x36,0x63,0,0,0,0},
{0,0,0,0,0,0,0,0xFF}
};

#define RGB(r,g,b) (((u32)(r) << 16) | ((u32)(g) << 8) | (u32)(b))

static inline void putpx(int x, int y, u32 c)
{
    if (x < 0 || y < 0 || (u32)x >= W || (u32)y >= H) return;
    *(u32 *)(fb + (u32)y * pitch + (u32)x * 4) = c;
}

static void rect(int x, int y, int w, int h, u32 c)
{
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            putpx(x + i, y + j, c);
}

static u32 lerp(u32 a, u32 b, int t, int max)
{
    u32 r = 0;
    for (int s = 16; s >= 0; s -= 8) {
        int ca = (a >> s) & 255, cb = (b >> s) & 255;
        r |= (u32)(ca + (cb - ca) * t / max) << s;
    }
    return r;
}

static void vgrad(int x, int y, int w, int h, u32 top, u32 bot)
{
    for (int j = 0; j < h; j++)
        rect(x, y + j, w, 1, lerp(top, bot, j, h > 1 ? h - 1 : 1));
}

static void glyph(int x, int y, char ch, u32 c)
{
    if (ch >= 'a' && ch <= 'z') ch -= 32;
    if (ch < 0x20 || ch > 0x5F) ch = '?';
    const u8 *g = font[ch - 0x20];
    for (int j = 0; j < 8; j++)
        for (int i = 0; i < 8; i++)
            if (g[j] & (1 << i)) putpx(x + i, y + j, c);
}

static void text(int x, int y, const char *s, u32 c)
{
    for (; *s; s++, x += 8) glyph(x, y, *s, c);
}

/* Fondo estilo "Bliss": cielo degradado y colina verde */
static void wallpaper(void)
{
    int horizon = (int)H * 55 / 100;
    vgrad(0, 0, (int)W, horizon, RGB(36, 110, 214), RGB(176, 214, 248));
    /* nubes simples */
    for (int n = 0; n < 5; n++) {
        int cx = 90 + n * 150, cy = 70 + (n * 37) % 90;
        for (int k = 0; k < 5; k++)
            for (int j = -10; j <= 10; j++)
                for (int i = -28; i <= 28; i++)
                    if ((i * i) / 9 + j * j < 100 - k * 4)
                        putpx(cx + i + k * 14, cy + j - (k & 1) * 6, RGB(250, 252, 255));
    }
    /* colina */
    for (int x = 0; x < (int)W; x++) {
        int top = horizon - 40 + (int)((x - (int)W / 3) * (x - (int)W / 3) / 2200) ;
        if (top > horizon + 60) top = horizon + 60;
        if (top < horizon - 40) top = horizon - 40;
        vgrad(x, top, 1, (int)H - top, RGB(94, 170, 40), RGB(30, 100, 20));
    }
}

static void icon(int x, int y, const char *label, u32 color)
{
    rect(x, y, 32, 32, color);
    rect(x, y, 32, 3, RGB(255, 255, 255));
    text(x - 4, y + 38, label, RGB(255, 255, 255));
}

static void window(int x, int y, int w, int h, const char *title)
{
    rect(x - 2, y - 2, w + 4, h + 4, RGB(0, 60, 200));
    vgrad(x, y, w, 26, RGB(0, 88, 238), RGB(0, 60, 200));
    text(x + 8, y + 9, title, RGB(255, 255, 255));
    /* botones de la ventana */
    rect(x + w - 24, y + 4, 18, 18, RGB(200, 60, 40));
    text(x + w - 19, y + 9, "X", RGB(255, 255, 255));
    rect(x + w - 46, y + 4, 18, 18, RGB(40, 100, 240));
    text(x + w - 41, y + 9, "_", RGB(255, 255, 255));
    rect(x, y + 26, w, h - 26, RGB(236, 233, 216));
}

static void taskbar(void)
{
    int ty = (int)H - 30;
    vgrad(0, ty, (int)W, 30, RGB(36, 110, 240), RGB(10, 60, 200));
    rect(0, ty, (int)W, 1, RGB(90, 150, 255));
    /* boton Inicio */
    vgrad(0, ty, 100, 30, RGB(80, 180, 70), RGB(40, 130, 36));
    text(30, ty + 11, "INICIO", RGB(255, 255, 255));
    rect(8, ty + 7, 16, 16, RGB(255, 255, 255));
    rect(9, ty + 8, 7, 7, RGB(240, 80, 40));
    rect(17, ty + 8, 6, 7, RGB(90, 200, 60));
    rect(9, ty + 16, 7, 6, RGB(60, 140, 240));
    rect(17, ty + 16, 6, 6, RGB(250, 210, 40));
    /* boton de ventana abierta */
    vgrad(110, ty + 3, 150, 24, RGB(60, 130, 245), RGB(30, 90, 220));
    text(120, ty + 11, "MI PC", RGB(255, 255, 255));
    /* bandeja del sistema */
    rect((int)W - 90, ty, 90, 30, RGB(16, 120, 230));
    text((int)W - 70, ty + 11, "22:33", RGB(255, 255, 255));
}

void kernel_main(u32 magic, struct mb_info *mb)
{
    if (magic != MULTIBOOT_MAGIC || !(mb->flags & (1u << 12)) || mb->fb_bpp != 32) {
        volatile u16 *vga = (volatile u16 *)0xB8000;
        const char *m = "WINXP-OS: se necesita framebuffer 32 bpp (GRUB multiboot).";
        for (int i = 0; m[i]; i++) vga[i] = (u16)(0x4F00 | (u8)m[i]);
        return;
    }
    fb = (u8 *)mb->fb_addr_lo;
    pitch = mb->fb_pitch; W = mb->fb_width; H = mb->fb_height;

    wallpaper();
    icon(30, 30, "MI PC", RGB(60, 120, 220));
    icon(30, 110, "PAPELERA", RGB(150, 150, 150));
    icon(30, 190, "DOCS", RGB(240, 200, 60));
    window((int)W / 2 - 170, (int)H / 2 - 110, 360, 210, "MI PC");
    text((int)W / 2 - 155, (int)H / 2 - 66, "WINXP-OS 0.1", RGB(0, 0, 0));
    text((int)W / 2 - 155, (int)H / 2 - 46, "KERNEL C + GNU TOOLCHAIN", RGB(0, 0, 0));
    text((int)W / 2 - 155, (int)H / 2 - 26, "ARRANQUE MULTIBOOT / GRUB", RGB(0, 0, 0));
    taskbar();

    for (;;) __asm__ volatile("hlt");
}
