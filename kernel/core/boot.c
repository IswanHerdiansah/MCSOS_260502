#include <mcsos/arch/limine.h>

/* Limine v8 base revision — wajib ada */
__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

/* Request framebuffer — opsional tapi direkomendasikan Limine v8 */
__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request fb_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
};

/* Anchor — wajib ada di Limine v8 */
__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;
