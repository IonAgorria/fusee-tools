
void main()
{
	//Reset to APX via PMC regs
	*((unsigned int*) (0x7000e450)) = 0x2;
	*((unsigned int*) (0x7000e400)) = 0x10;
}
