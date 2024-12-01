/*
 * Modified driver for NVIDIA Tegra AES hardware engine residing inside the
 * Bit Stream Engine for Video (BSEV) hardware block.
 *
 * The programming sequence for this engine is with the help
 * of commands which travel via a command queue residing between the
 * CPU and the BSEV block. The BSEV engine has an internal RAM (VRAM)
 * where the final input plaintext, keys and the IV have to be copied
 * before starting the encrypt/decrypt operation.
 *
 * Copyright (c) 2010, NVIDIA Corporation.
 * Copyright (c) 2024, Ion Agorria.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __CRYPTODEV_TEGRA_AES_H
#define __CRYPTODEV_TEGRA_AES_H

#include "common.h"

#define TEGRA_BSEA_BASE 0x60011000
#define TEGRA_VDE_BASE 0x6001A000

#define TEGRA_AES_ICMDQUE_WR			0x1000
#define TEGRA_AES_CMDQUE_CONTROL		0x1008
#define TEGRA_AES_INTR_STATUS			0x1018
#define TEGRA_AES_INT_ENB			0x1040
#define TEGRA_AES_CONFIG			0x1044
#define TEGRA_AES_IRAM_ACCESS_CFG		0x10A0
#define TEGRA_AES_SECURE_DEST_ADDR		0x1100
#define TEGRA_AES_SECURE_INPUT_SELECT		0x1104
#define TEGRA_AES_SECURE_CONFIG			0x1108
#define TEGRA_AES_SECURE_CONFIG_EXT		0x110C
#define TEGRA_AES_SECURE_SECURITY		0x1110
#define TEGRA_AES_SECURE_HASH_RESULT0		0x1120
#define TEGRA_AES_SECURE_HASH_RESULT1		0x1124
#define TEGRA_AES_SECURE_HASH_RESULT2		0x1128
#define TEGRA_AES_SECURE_HASH_RESULT3		0x112C
#define TEGRA_AES_SECURE_SEC_SEL0		0x1140
#define TEGRA_AES_SECURE_SEC_SEL1		0x1144
#define TEGRA_AES_SECURE_SEC_SEL2		0x1148
#define TEGRA_AES_SECURE_SEC_SEL3		0x114C
#define TEGRA_AES_SECURE_SEC_SEL4		0x1150
#define TEGRA_AES_SECURE_SEC_SEL5		0x1154
#define TEGRA_AES_SECURE_SEC_SEL6		0x1158
#define TEGRA_AES_SECURE_SEC_SEL7		0x115C

/* interrupt status reg masks and shifts */
#define TEGRA_AES_ENGINE_BUSY_FIELD		BIT(0)
#define TEGRA_AES_ICQ_EMPTY_FIELD		BIT(3)
#define TEGRA_AES_DMA_BUSY_FIELD		BIT(23)

/* secure select reg masks and shifts */
#define TEGRA_AES_SECURE_SEL0_KEYREAD_ENB0_FIELD	BIT(0)

/* secure config ext masks and shifts */
#define TEGRA_AES_SECURE_KEY_SCH_DIS_FIELD	BIT(15)

/* secure config masks and shifts */
#define TEGRA_AES_SECURE_KEY_INDEX_SHIFT	20
#define TEGRA_AES_SECURE_KEY_INDEX_FIELD	(0x1F << TEGRA_AES_SECURE_KEY_INDEX_SHIFT)
#define TEGRA_AES_SECURE_BLOCK_CNT_SHIFT	0
#define TEGRA_AES_SECURE_BLOCK_CNT_FIELD	(0xFFFFF << TEGRA_AES_SECURE_BLOCK_CNT_SHIFT)

/* stream interface select masks and shifts */
#define TEGRA_AES_CMDQ_CTRL_UCMDQEN_FIELD	BIT(0)
#define TEGRA_AES_CMDQ_CTRL_ICMDQEN_FIELD	BIT(1)
#define TEGRA_AES_CMDQ_CTRL_SRC_STM_SEL_FIELD	BIT(4)
#define TEGRA_AES_CMDQ_CTRL_DST_STM_SEL_FIELD	BIT(5)

/* config register masks and shifts */
#define TEGRA_AES_CONFIG_ENDIAN_ENB_FIELD	BIT(10)
#define TEGRA_AES_CONFIG_MODE_SEL_SHIFT		0
#define TEGRA_AES_CONFIG_MODE_SEL_FIELD		(0x1F << TEGRA_AES_CONFIG_MODE_SEL_SHIFT)

/* extended config */
#define TEGRA_AES_SECURE_OFFSET_CNT_SHIFT	24
#define TEGRA_AES_SECURE_OFFSET_CNT_FIELD	(0xFF << TEGRA_AES_SECURE_OFFSET_CNT_SHIFT)
#define TEGRA_AES_SECURE_KEYSCHED_GEN_FIELD	BIT(15)

/* init vector select */
#define TEGRA_AES_SECURE_IV_SELECT_SHIFT	10
#define TEGRA_AES_SECURE_IV_SELECT_FIELD	BIT(10)

/* secure engine input */
#define TEGRA_AES_SECURE_INPUT_ALG_SEL_SHIFT	28
#define TEGRA_AES_SECURE_INPUT_ALG_SEL_FIELD	(0xF << TEGRA_AES_SECURE_INPUT_ALG_SEL_SHIFT)
#define TEGRA_AES_SECURE_INPUT_KEY_LEN_SHIFT	16
#define TEGRA_AES_SECURE_INPUT_KEY_LEN_FIELD	(0xFFF << TEGRA_AES_SECURE_INPUT_KEY_LEN_SHIFT)
#define TEGRA_AES_SECURE_RNG_ENB_FIELD		BIT(11)
#define TEGRA_AES_SECURE_CORE_SEL_SHIFT		9
#define TEGRA_AES_SECURE_CORE_SEL_FIELD		BIT(9)
#define TEGRA_AES_SECURE_VCTRAM_SEL_SHIFT	7
#define TEGRA_AES_SECURE_VCTRAM_SEL_FIELD	(0x3 << TEGRA_AES_SECURE_VCTRAM_SEL_SHIFT)
#define TEGRA_AES_SECURE_INPUT_SEL_SHIFT	5
#define TEGRA_AES_SECURE_INPUT_SEL_FIELD	(0x3 << TEGRA_AES_SECURE_INPUT_SEL_SHIFT)
#define TEGRA_AES_SECURE_XOR_POS_SHIFT		3
#define TEGRA_AES_SECURE_XOR_POS_FIELD		(0x3 << TEGRA_AES_SECURE_XOR_POS_SHIFT)
#define TEGRA_AES_SECURE_HASH_ENB_FIELD		BIT(2)
#define TEGRA_AES_SECURE_ON_THE_FLY_FIELD	BIT(0)

/* interrupt error mask */
#define TEGRA_AES_INT_ERROR_MASK		0xFFF000

#define TEGRA_AES_SBK_SLOT_NUM		0
#define TEGRA_AES_SSK_SLOT_NUM		4

#define FLAGS_MODE_MASK			0xFF
#define FLAGS_ENCRYPT			BIT(0)
#define FLAGS_CBC			BIT(1)
#define FLAGS_GIV			BIT(2)
#define FLAGS_RNG			BIT(3)
#define FLAGS_OFB			BIT(4)


/*
 * The key table length is 64 bytes
 * (This includes first upto 32 bytes key + 16 bytes original initial vector
 * and 16 bytes updated initial vector)
 */
#define AES_HW_KEY_TABLE_LENGTH_BYTES 64

/*
 * The memory being used is divides as follows:
 * 1. Key - 32 bytes
 * 2. Original IV - 16 bytes
 * 3. Updated IV - 16 bytes
 * 4. Key schedule - 256 bytes
 *
 * 1+2+3 constitute the hw key table.
 */
#define AES_HW_KEY_SIZE 32
#define AES_HW_IV_SIZE 16
#define AES_HW_KEYSCHEDULE_LEN 256
#define AES_IVKEY_SIZE (AES_HW_KEY_TABLE_LENGTH_BYTES + AES_HW_KEYSCHEDULE_LEN)

/* Define commands required for AES operation */
enum {
    CMD_BLKSTARTENGINE = 0x0E,
    CMD_DMASETUP = 0x10,
    CMD_DMACOMPLETE = 0x11,
    CMD_SETTABLE = 0x15,
    CMD_MEMDMAVD = 0x22,
};

/* Define sub-commands */
enum {
    SUBCMD_VRAM_SEL = 0x1,
    SUBCMD_CRYPTO_TABLE_SEL = 0x3,
    SUBCMD_KEY_TABLE_SEL = 0x8,
};

/* memdma_vd command */
#define MEMDMA_DIR_DTOVRAM		0 /* sdram -> vram */
#define MEMDMA_DIR_VTODRAM		1 /* vram -> sdram */
#define MEMDMA_DIR_SHIFT		25
#define MEMDMA_NUM_WORDS_SHIFT		12

/* command queue bit shifts */
enum {
    CMDQ_KEYTABLEADDR_SHIFT = 0,
    CMDQ_KEYTABLEID_SHIFT = 17,
    CMDQ_VRAMSEL_SHIFT = 23,
    CMDQ_TABLESEL_SHIFT = 24,
    CMDQ_DIRECTION_SHIFT = 25,
    CMDQ_OPCODE_SHIFT = 26,
};

#define TEGRA_AES_NR_KEYSLOTS		8
#define TEGRA_AES_QUEUE_LENGTH	50

struct tegra_aes_dev {
    uint8_t* io_base;
    uint32_t keylen;
};

static inline uint32_t tegra_aes_readl(struct tegra_aes_dev *dd, uint32_t offset)
{
    return reg_read(dd->io_base, offset);
}

static inline void tegra_aes_writel(struct tegra_aes_dev *dd, uint32_t val, uint32_t offset)
{
    reg_write(dd->io_base, offset, val);
}

void tegra_aes_wait_for_idle(struct tegra_aes_dev* dd, uint8_t dma_wait) {
    uint32_t value;
    do {
        value = tegra_aes_readl(dd, TEGRA_AES_INTR_STATUS);
    } while ((value & TEGRA_AES_ENGINE_BUSY_FIELD)
         || !(value & TEGRA_AES_ICQ_EMPTY_FIELD)
         || (dma_wait && (value & TEGRA_AES_DMA_BUSY_FIELD))
    );
}

uint32_t tegra_aes_read_error(struct tegra_aes_dev* dd) {
    uint32_t value = tegra_aes_readl(dd, TEGRA_AES_INTR_STATUS) & TEGRA_AES_INT_ERROR_MASK;
    if (value) {
        tegra_aes_writel(dd, TEGRA_AES_INTR_STATUS, TEGRA_AES_INT_ERROR_MASK);
    }
    return value;
}

static void tegra_aes_enable_icmdqen(struct tegra_aes_dev* dd) {
    uint32_t value = tegra_aes_readl(dd, TEGRA_AES_CMDQUE_CONTROL);
    /* access SDRAM through AHB */
    value &= ~TEGRA_AES_CMDQ_CTRL_SRC_STM_SEL_FIELD;
    value &= ~TEGRA_AES_CMDQ_CTRL_DST_STM_SEL_FIELD;
    value |= TEGRA_AES_CMDQ_CTRL_SRC_STM_SEL_FIELD |
             TEGRA_AES_CMDQ_CTRL_DST_STM_SEL_FIELD |
             TEGRA_AES_CMDQ_CTRL_ICMDQEN_FIELD;
    tegra_aes_writel(dd, value, TEGRA_AES_CMDQUE_CONTROL);
    tegra_aes_dbg(dd, "cmd_q_ctrl=0x%x", value);
}

static void tegra_aes_start_crypt(
        struct tegra_aes_dev* dd,
        const uint8_t* in_addr, uint8_t* out_addr,
        int nblocks, int mode, uint8_t upd_iv_bool
) {
    tegra_aes_dbg(dd, "tegra_aes_start_crypt 0x%x -> 0x%x (%d)", in_addr, out_addr, nblocks);
    const uint32_t AES_HW_MAX_ICQ_LENGTH = 5;
    uint32_t cmdq[AES_HW_MAX_ICQ_LENGTH];

    tegra_aes_enable_icmdqen(dd);

    cmdq[0] = CMD_DMASETUP << CMDQ_OPCODE_SHIFT;
    cmdq[1] = (uint32_t) in_addr;
    cmdq[2] = CMD_BLKSTARTENGINE << CMDQ_OPCODE_SHIFT | (nblocks-1);
    cmdq[3] = CMD_DMACOMPLETE << CMDQ_OPCODE_SHIFT;


    uint32_t value = (0x1 << TEGRA_AES_SECURE_INPUT_ALG_SEL_SHIFT) |
            ((dd->keylen * 8) << TEGRA_AES_SECURE_INPUT_KEY_LEN_SHIFT) |
            ((uint32_t)upd_iv_bool << TEGRA_AES_SECURE_IV_SELECT_SHIFT);

    if (mode & FLAGS_CBC) {
        value |= (
                (((mode & FLAGS_ENCRYPT) ? 2 : 3) << TEGRA_AES_SECURE_XOR_POS_SHIFT) |
                (((mode & FLAGS_ENCRYPT) ? 2 : 3) << TEGRA_AES_SECURE_VCTRAM_SEL_SHIFT) |
                ((mode & FLAGS_ENCRYPT) ? 1 : 0) << TEGRA_AES_SECURE_CORE_SEL_SHIFT
        );
    } else if (mode & FLAGS_OFB) {
        value |= (
                (TEGRA_AES_SECURE_XOR_POS_FIELD) |
                (2 << TEGRA_AES_SECURE_INPUT_SEL_SHIFT) |
                (TEGRA_AES_SECURE_CORE_SEL_FIELD)
        );
    } else if (mode & FLAGS_RNG) {
        value |= (
            ((mode & FLAGS_ENCRYPT) ? 1 : 0) << TEGRA_AES_SECURE_CORE_SEL_SHIFT |
            TEGRA_AES_SECURE_RNG_ENB_FIELD
        );
    } else {
        value |= (
            ((mode & FLAGS_ENCRYPT) ? 1 : 0) << TEGRA_AES_SECURE_CORE_SEL_SHIFT
        );
    }

    tegra_aes_dbg(dd, "secure_in_sel=0x%x", value);
    tegra_aes_writel(dd, value, TEGRA_AES_SECURE_INPUT_SELECT);
    tegra_aes_writel(dd, (uint32_t) out_addr, TEGRA_AES_SECURE_DEST_ADDR);

    for (int i = 0; i < AES_HW_MAX_ICQ_LENGTH - 1; i++) {
        tegra_aes_wait_for_idle(dd, 0);
        tegra_aes_writel(dd, cmdq[i], TEGRA_AES_ICMDQUE_WR);
    }

    tegra_aes_wait_for_idle(dd, 1);

    tegra_aes_writel(dd, cmdq[AES_HW_MAX_ICQ_LENGTH - 1], TEGRA_AES_ICMDQUE_WR);
    tegra_aes_wait_for_idle(dd, 1);
}

/* select the key slot */
static void tegra_aes_select_key_slot(struct tegra_aes_dev *dd, uint8_t slot) {
    tegra_aes_dbg(dd, "aes_select_key_slot=0x%x", slot);
    uint32_t value = tegra_aes_readl(dd, TEGRA_AES_SECURE_CONFIG);
    value &= ~TEGRA_AES_SECURE_KEY_INDEX_FIELD;
    value |= (slot << TEGRA_AES_SECURE_KEY_INDEX_SHIFT);
    tegra_aes_writel(dd, value, TEGRA_AES_SECURE_CONFIG);
}

/* copy the data from sdram to vram */
static void tegra_aes_copy_to_vram(struct tegra_aes_dev *dd, const uint8_t* data, uint32_t data_words) {
    tegra_aes_dbg(dd, "aes_copy_to_vram=0x%p words=%d", data, data_words);
    uint32_t cmdq[2];
    cmdq[0] = CMD_MEMDMAVD << CMDQ_OPCODE_SHIFT |
              MEMDMA_DIR_DTOVRAM << MEMDMA_DIR_SHIFT |
              data_words << MEMDMA_NUM_WORDS_SHIFT;
    cmdq[1] = (uint32_t) data;

    tegra_aes_writel(dd, cmdq[0], TEGRA_AES_ICMDQUE_WR);
    tegra_aes_writel(dd, cmdq[1], TEGRA_AES_ICMDQUE_WR);

    tegra_aes_wait_for_idle(dd, 1);
    tegra_aes_dbg(dd, "aes_copy_to_vram=0x%p complete", data);
}

/* copy the data from vram to sdram */
static void tegra_aes_copy_from_vram(struct tegra_aes_dev *dd, uint8_t* data, uint32_t data_words) {
    tegra_aes_dbg(dd, "aes_copy_from_vram=0x%p words=%d", data, data_words);
    uint32_t cmdq[2];
    cmdq[0] = CMD_MEMDMAVD << CMDQ_OPCODE_SHIFT |
              MEMDMA_DIR_VTODRAM << MEMDMA_DIR_SHIFT |
              data_words << MEMDMA_NUM_WORDS_SHIFT;
    cmdq[1] = (uint32_t) data;

    tegra_aes_writel(dd, cmdq[0], TEGRA_AES_ICMDQUE_WR);
    tegra_aes_writel(dd, cmdq[1], TEGRA_AES_ICMDQUE_WR);

    tegra_aes_wait_for_idle(dd, 1);
    tegra_aes_dbg(dd, "aes_copy_from_vram=0x%p complete", data);
}

/* settable command to get key into internal registers */
static void tegra_aes_set_slot_key_from_vram(struct tegra_aes_dev *dd, uint8_t slot) {
    tegra_aes_dbg(dd, "aes_set_slot_key_from_vram=0x%x complete", slot);
    uint32_t value = CMD_SETTABLE << CMDQ_OPCODE_SHIFT |
                     SUBCMD_CRYPTO_TABLE_SEL << CMDQ_TABLESEL_SHIFT |
                     SUBCMD_VRAM_SEL << CMDQ_VRAMSEL_SHIFT |
                     (SUBCMD_KEY_TABLE_SEL | slot) << CMDQ_KEYTABLEID_SHIFT;
    tegra_aes_writel(dd, value, TEGRA_AES_ICMDQUE_WR);

    tegra_aes_wait_for_idle(dd, 0);
}

static void tegra_aes_clear_vram(struct tegra_aes_dev *dd) {
    tegra_aes_dbg(dd, "tegra_aes_clear_vram");
    uint32_t value = 0x21 << CMDQ_OPCODE_SHIFT |
                     (0x1fff << 12);
    tegra_aes_writel(dd, value, TEGRA_AES_ICMDQUE_WR);

    tegra_aes_wait_for_idle(dd, 1);
}

static void tegra_aes_test(struct tegra_aes_dev *dd, uint8_t* data, uint32_t data_words) {
    tegra_aes_dbg(dd, "tegra_aes_test=0x%p words=%d", data, data_words);

    tegra_aes_writel(dd, (
        (0x20 << CMDQ_OPCODE_SHIFT)
       |(0x3f0 << 16) //BSE addr
       |(0xDEAD) //Data
    ), TEGRA_AES_ICMDQUE_WR);
    //tegra_aes_writel(dd, (uint32_t) data, TEGRA_AES_ICMDQUE_WR);

    tegra_aes_wait_for_idle(dd, 1);
    tegra_aes_dbg(dd, "tegra_aes_test=0x%p complete", data);
}


static void tegra_aes_init(struct tegra_aes_dev *dd)
{
    if (!dd->io_base) {
        tegra_aes_dbg(dd, "error! io_base not set");
        return;
    }

    /* reset all the interrupt bits */
    tegra_aes_writel(dd, 0xFFFFFFFF, TEGRA_AES_INTR_STATUS);

    /* disable interrupts */
    tegra_aes_writel(dd, 0x0, TEGRA_AES_INT_ENB);

    /* enable key schedule generation in hardware */
    uint32_t value = tegra_aes_readl(dd, TEGRA_AES_SECURE_CONFIG_EXT);
    value &= ~TEGRA_AES_SECURE_KEY_SCH_DIS_FIELD;
    tegra_aes_writel(dd, value, TEGRA_AES_SECURE_CONFIG_EXT);

    tegra_aes_dbg(dd, "init done");
}

#endif
