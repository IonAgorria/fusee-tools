// SPDX-License-Identifier: GPL-2.0+

// Taken from uboot aes driver, do some "bridging"
typedef uint32_t u32;
typedef uint8_t u8;

#define log_debug(...)
#define log_warning(...)
#define log_err(...)
#define EINVAL 2
#define BIT(nr) (1UL << (nr))
#define writel(value, addr) reg_write(addr, 0, value)
#define readl(addr) reg_read(addr, 0)
#define mdelay msleep

enum {
	AES128_KEY_LENGTH	= 128 / 8,
	AES256_KEY_LENGTH	= 256 / 8,
	AES_BLOCK_LENGTH	= 128 / 8,
};

const u32 TEGRA_AES_DMA_BUFFER_SIZE = 0x4000 / AES_BLOCK_LENGTH;
const u32 TEGRA_AES_HW_MAX_KEY_LENGTH = AES256_KEY_LENGTH;
const u32 TEGRA_AES_HW_TABLE_LENGTH = TEGRA_AES_HW_MAX_KEY_LENGTH + AES_BLOCK_LENGTH * 2;

#define TEGRA_AES_BUSY_TIMEOUT_MS               1000

//Registers
#define TEGRA_AES_ICMDQUE_WR                    0x000
#define TEGRA_AES_CMDQUE_CONTROL                0x008
#define TEGRA_AES_INTR_STATUS                   0x018
#define TEGRA_AES_INT_ENB                       0x040
#define TEGRA_AES_BSE_CFG                       0x044
#define TEGRA_AES_IRAM_ACCESS_CFG               0x0A0
#define TEGRA_AES_SECURE_DEST_ADDR              0x100
#define TEGRA_AES_SECURE_INPUT_SELECT           0x104
#define TEGRA_AES_SECURE_CONFIG                 0x108
#define TEGRA_AES_SECURE_CONFIG_EXT             0x10C

//Register field macros
#define TEGRA_AES_ENGINE_BUSY_FIELD             BIT(0)
#define TEGRA_AES_ICQ_EMPTY_FIELD               BIT(3)
#define TEGRA_AES_DMA_BUSY_FIELD                BIT(23)
#define TEGRA_AES_SECURE_KEY_SCH_DIS_FIELD      BIT(15)
#define TEGRA_AES_KEYTABLEADDR_FIELD            (BIT(17) - 1)
#define TEGRA_AES_SECURE_KEY_INDEX_SHIFT        20
#define TEGRA_AES_SECURE_KEY_INDEX_FIELD        (0x1F << TEGRA_AES_SECURE_KEY_INDEX_SHIFT)
#define TEGRA_AES_SECURE_CTR_CNT_SHIFT          16
#define TEGRA_AES_SECURE_CTR_CNT_FIELD          (0xFFFF << TEGRA_AES_SECURE_CTR_CNT_SHIFT)
#define TEGRA_AES_BSE_MODE_FIELD                0x1F
#define TEGRA_AES_BSE_LITTLE_ENDIAN_FIELD       BIT(10)
#define TEGRA_AES_CMDQ_OPCODE_SHIFT             26
#define TEGRA_AES_CMDQ_CTRL_ICMDQEN_FIELD       BIT(1)
#define TEGRA_AES_CMDQ_CTRL_SRC_STM_SEL_FIELD   BIT(4)
#define TEGRA_AES_CMDQ_CTRL_DST_STM_SEL_FIELD   BIT(5)
#define TEGRA_AES_SECURE_INPUT_ALG_SEL_SHIFT    28
#define TEGRA_AES_SECURE_INPUT_KEY_LEN_SHIFT    16
#define TEGRA_AES_SECURE_INPUT_IV_FIELD         BIT(10)
#define TEGRA_AES_SECURE_INPUT_HASH_ENB_FIELD   BIT(2)
#define TEGRA_AES_SECURE_CORE_SEL_SHIFT         9
#define TEGRA_AES_SECURE_VCTRAM_SEL_SHIFT       7
#define TEGRA_AES_SECURE_XOR_POS_SHIFT          3
#define TEGRA_AES_INT_ERROR_MASK                0x6FF000

//Commands for BSEV/BSEA
#define TEGRA_AES_CMD_BLKSTARTENGINE            0x0E
#define TEGRA_AES_CMD_DMASETUP                  0x10
#define TEGRA_AES_CMD_DMACOMPLETE               0x11
#define TEGRA_AES_CMD_SETTABLE                  0x15

//Flags for mode
#define TEGRA_AES_MODE_ENCRYPT                 BIT(0)
#define TEGRA_AES_MODE_CBC                     BIT(1)
#define TEGRA_AES_MODE_UPDATE_IV               BIT(2)
#define TEGRA_AES_MODE_HASH                    BIT(3)

struct tegra_aes_priv {
	void *regs;
	void *iram_addr;
	u8 current_key_size;
};

bool tegra_aes_is_busy(struct tegra_aes_priv *priv, bool dma_wait) {
	u32 value = readl(priv->regs + TEGRA_AES_INTR_STATUS);
	bool engine_busy = value & TEGRA_AES_ENGINE_BUSY_FIELD;
	bool non_empty_queue = !(value & TEGRA_AES_ICQ_EMPTY_FIELD);
	bool dma_busy = dma_wait && (value & TEGRA_AES_DMA_BUSY_FIELD);
	//log_debug("%s - e:%d q:%d dma:%d\n", __func__, engine_busy, non_empty_queue, dma_busy);
	return engine_busy || non_empty_queue || dma_busy;
}

u32 tegra_aes_check_error(struct tegra_aes_priv *priv) {
	u32 value = readl(priv->regs + TEGRA_AES_INTR_STATUS) & TEGRA_AES_INT_ERROR_MASK;
	if (value) {
		writel(TEGRA_AES_INT_ERROR_MASK, priv->regs + TEGRA_AES_INTR_STATUS);
		log_err("%s 0x%x\n", __func__, value);
	}
	return value;
}

int tegra_aes_wait_for_idle_dma(struct tegra_aes_priv *priv, bool dma_wait) {
	uint32_t start = get_usec();
	while (tegra_aes_is_busy(priv, dma_wait)) {
		if ((TEGRA_AES_BUSY_TIMEOUT_MS * 1000) < (get_usec() - start)) {
			log_err("%s: TIMEOUT!!!\n", __func__);
			break;
		}
		mdelay(5);
	}

	if (tegra_aes_check_error(priv))
		return -1;

	return 0;
}

int tegra_aes_wait_for_idle(struct tegra_aes_priv *priv) {
	return tegra_aes_wait_for_idle_dma(priv, 1);
}

int tegra_aes_configure(struct tegra_aes_priv *priv) {
	u32 value;
	if (tegra_aes_wait_for_idle(priv))
		return -1;

	//IRAM config
	writel(0, priv->regs + TEGRA_AES_IRAM_ACCESS_CFG);

	//Reset interrupts bits, or engine will hang on next operation
	writel(0xFFFFFFFF, priv->regs + TEGRA_AES_INTR_STATUS);

	//Set interrupts
	writel(0, priv->regs + TEGRA_AES_INT_ENB);

	//Configure CMDQUE
	value = readl(priv->regs + TEGRA_AES_CMDQUE_CONTROL);
	value |= TEGRA_AES_CMDQ_CTRL_SRC_STM_SEL_FIELD |
	         TEGRA_AES_CMDQ_CTRL_DST_STM_SEL_FIELD |
	         TEGRA_AES_CMDQ_CTRL_ICMDQEN_FIELD;
	writel(value, priv->regs + TEGRA_AES_CMDQUE_CONTROL);

	value = readl(priv->regs + TEGRA_AES_SECURE_CONFIG_EXT);
	value &= ~TEGRA_AES_SECURE_CTR_CNT_FIELD;
	writel(value, priv->regs + TEGRA_AES_SECURE_CONFIG_EXT);

	//Configure BSE
	value = readl(priv->regs + TEGRA_AES_BSE_CFG);
	value &= ~TEGRA_AES_BSE_MODE_FIELD;
	value |= TEGRA_AES_BSE_LITTLE_ENDIAN_FIELD;
	writel(value, priv->regs + TEGRA_AES_BSE_CFG);

	return 0;
}

int tegra_aes_select_key_slot(struct tegra_aes_priv *priv, u32 key_size, u8 slot) {
	if (tegra_aes_wait_for_idle(priv))
		return -1;

	if (key_size < (AES128_KEY_LENGTH * 8) || key_size > (TEGRA_AES_HW_MAX_KEY_LENGTH * 8))
		return -EINVAL;

	priv->current_key_size = key_size;

	// Select the key slot
	u32 value = readl(priv->regs + TEGRA_AES_SECURE_CONFIG);
	value &= ~TEGRA_AES_SECURE_KEY_INDEX_FIELD;
	value |= (slot << TEGRA_AES_SECURE_KEY_INDEX_SHIFT);
	writel(value, priv->regs + TEGRA_AES_SECURE_CONFIG);

	return 0;
}

int tegra_aes_call_engine(struct tegra_aes_priv *priv, u8 *src, u8 *dst,
                          u32 nblocks, u32 mode)
{
	u32 value;
	const u32 ICMDQ_LENGTH = 4;
	u32 cmdq[ICMDQ_LENGTH];

	//log_debug("%s: 0x%p -> 0x%p blocks %d mode 0x%x\n", __func__, src, dst, nblocks, mode);

	if (nblocks == 0) {
		log_warning("%s: called with 0 blocks!\n", __func__);
		return -1;
	}

	if (tegra_aes_configure(priv))
		return -1;

	//Configure Secure Input
	value = 1 << TEGRA_AES_SECURE_INPUT_ALG_SEL_SHIFT |
	        priv->current_key_size << TEGRA_AES_SECURE_INPUT_KEY_LEN_SHIFT;

	if (mode & TEGRA_AES_MODE_UPDATE_IV)
		value |= TEGRA_AES_SECURE_INPUT_IV_FIELD;
	if (mode & TEGRA_AES_MODE_HASH)
		value |= TEGRA_AES_SECURE_INPUT_HASH_ENB_FIELD;
	if (mode & TEGRA_AES_MODE_CBC) {
		//CBC
		value |= ((mode & TEGRA_AES_MODE_ENCRYPT) ? 2 : 3) << TEGRA_AES_SECURE_XOR_POS_SHIFT;
		value |= ((mode & TEGRA_AES_MODE_ENCRYPT) ? 2 : 3) << TEGRA_AES_SECURE_VCTRAM_SEL_SHIFT;
		value |= ((mode & TEGRA_AES_MODE_ENCRYPT) ? 1 : 0) << TEGRA_AES_SECURE_CORE_SEL_SHIFT;
	} else {
		//ECB
		value |= ((mode & TEGRA_AES_MODE_ENCRYPT) ? 1 : 0) << TEGRA_AES_SECURE_CORE_SEL_SHIFT;
	}

	writel(value, priv->regs + TEGRA_AES_SECURE_INPUT_SELECT);

	//Set destination address (doing in-place at IRAM)
	writel((u32) priv->iram_addr, priv->regs + TEGRA_AES_SECURE_DEST_ADDR);

	//Copy src data to IRAM
	if (src != priv->iram_addr)
		memcpy(priv->iram_addr, src, nblocks * AES_BLOCK_LENGTH);

	//Run ICMD commands
	cmdq[0] = TEGRA_AES_CMD_DMASETUP << TEGRA_AES_CMDQ_OPCODE_SHIFT;
	cmdq[1] = (u32) priv->iram_addr; //Source address
	cmdq[2] = TEGRA_AES_CMD_BLKSTARTENGINE << TEGRA_AES_CMDQ_OPCODE_SHIFT | (nblocks - 1);
	cmdq[3] = TEGRA_AES_CMD_DMACOMPLETE << TEGRA_AES_CMDQ_OPCODE_SHIFT;
	for (int i = 0; i < ICMDQ_LENGTH; i++) {
		tegra_aes_wait_for_idle_dma(priv, (ICMDQ_LENGTH - 1) == i);
		writel(cmdq[i], priv->regs + TEGRA_AES_ICMDQUE_WR);
	}
	if (tegra_aes_wait_for_idle(priv))
		return -1;

	//Put the result from IRAM to destination if not hashing
	if (dst != priv->iram_addr && !(mode & TEGRA_AES_MODE_HASH))
		memcpy(dst, priv->iram_addr, nblocks * AES_BLOCK_LENGTH);

	return 0;
}

int tegra_aes_process_blocks(struct tegra_aes_priv *priv, u8 *iv,
                             u8 *src, u8 *dst, u32 num_aes_blocks,
                             u32 mode)
{
	log_debug("%s: 0x%p -> 0x%p blocks %d mode 0x%x\n", __func__, src, dst, num_aes_blocks, mode);
	if (num_aes_blocks == 0) {
		log_warning("%s: called with 0 blocks!\n", __func__);
		return -1;
	}

	//Load initial IV if CBC mode
	if (mode & TEGRA_AES_MODE_CBC) {
		if (tegra_aes_call_engine(priv, iv, priv->iram_addr, 1, TEGRA_AES_MODE_CBC))
			return -1;

		//Add update IV flag
		mode |= TEGRA_AES_MODE_UPDATE_IV;
	}

	//Process blocks by calling engine several times per dma buffer size
	while (0 < num_aes_blocks) {
		u32 blocks = min(num_aes_blocks, TEGRA_AES_DMA_BUFFER_SIZE);
		if (tegra_aes_call_engine(priv, src, dst, blocks, mode))
			return -1;
		num_aes_blocks -= blocks;
		src += blocks * AES_BLOCK_LENGTH;
		dst += blocks * AES_BLOCK_LENGTH;
	}

	return 0;
}

int tegra_aes_probe(struct tegra_aes_priv *priv)
{
	u32 value;
	log_debug("%s: start\n", __func__);

	priv->current_key_size = AES128_KEY_LENGTH;
	priv->regs = (void*) 0x6001b000;
	priv->iram_addr = (void*) 0x40004000;

	//Enable key schedule generation in hardware
	value = readl(priv->regs + TEGRA_AES_SECURE_CONFIG_EXT);
	value &= ~TEGRA_AES_SECURE_KEY_SCH_DIS_FIELD;
	writel(value, priv->regs + TEGRA_AES_SECURE_CONFIG_EXT);

	log_debug("%s: OK\n", __func__);

	return 0;
}

int tegra_aes_set_key_for_key_slot(struct tegra_aes_priv *priv, u32 key_size, u8 *key, u8 *iv, u8 slot) {
	const u8 SUBCMD_CRYPTO_TABLE_SEL = 0x3;
	const u8 SUBCMD_KEY_TABLE_SEL = 0x8;
	const u8 CMDQ_KEYTABLEADDR_SHIFT = 0;
	const u8 CMDQ_KEYTABLEID_SHIFT = 17;
	const u8 CMDQ_TABLESEL_SHIFT = 24;
	u32 value;
	u32 addr;

	log_debug("%s: slot %d\n", __func__, slot);
	if (tegra_aes_configure(priv))
		return -1;

	if (key_size < (AES128_KEY_LENGTH * 8) || key_size > (TEGRA_AES_HW_MAX_KEY_LENGTH * 8))
		return -EINVAL;

	//Clear and copy data to IRAM
	memset(priv->iram_addr, 0, TEGRA_AES_HW_TABLE_LENGTH);
	memcpy(priv->iram_addr, key, key_size / 8);
	memcpy(priv->iram_addr + AES256_KEY_LENGTH, iv, AES_BLOCK_LENGTH);

	//Mask the addr
	addr = ((u32) priv->iram_addr) & TEGRA_AES_KEYTABLEADDR_FIELD;

	//Command for engine to load AES key from IRAM
	value = TEGRA_AES_CMD_SETTABLE << TEGRA_AES_CMDQ_OPCODE_SHIFT |
	        SUBCMD_CRYPTO_TABLE_SEL << CMDQ_TABLESEL_SHIFT |
	        (SUBCMD_KEY_TABLE_SEL | slot) << CMDQ_KEYTABLEID_SHIFT |
	        addr << CMDQ_KEYTABLEADDR_SHIFT;
	writel(value, priv->regs + TEGRA_AES_ICMDQUE_WR);

	return tegra_aes_wait_for_idle(priv);
}
