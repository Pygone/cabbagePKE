
./obj/riscv-pke:     file format elf64-littleriscv


Disassembly of section .text:

0000000080000000 <elf_fpread>:
}

//
// actual file reading, using the spike file interface.
//
static uint64 elf_fpread(elf_ctx *ctx, void *dest, uint64 nb, uint64 offset) {
    80000000:	1141                	addi	sp,sp,-16
    80000002:	e406                	sd	ra,8(sp)
  elf_info *msg = (elf_info *)ctx->info;
    80000004:	611c                	ld	a5,0(a0)
  // call spike file utility to load the content of elf file into memory.
  // spike_file_pread will read the elf file (msg->f) from offset to memory (indicated by
  // *dest) for nb bytes.
  return spike_file_pread(msg->f, dest, nb, offset);
    80000006:	6388                	ld	a0,0(a5)
    80000008:	5ba040ef          	jal	800045c2 <spike_file_pread>
}
    8000000c:	60a2                	ld	ra,8(sp)
    8000000e:	0141                	addi	sp,sp,16
    80000010:	8082                	ret

0000000080000012 <elf_alloc_mb>:
static void *elf_alloc_mb(elf_ctx *ctx, uint64 elf_pa, uint64 elf_va, uint64 size) {
    80000012:	1101                	addi	sp,sp,-32
    80000014:	ec06                	sd	ra,24(sp)
    80000016:	e822                	sd	s0,16(sp)
    80000018:	e426                	sd	s1,8(sp)
    8000001a:	e04a                	sd	s2,0(sp)
  elf_info *msg = (elf_info *)ctx->info;
    8000001c:	00053903          	ld	s2,0(a0)
  kassert(size < PGSIZE);
    80000020:	6785                	lui	a5,0x1
    80000022:	04f6f263          	bgeu	a3,a5,80000066 <elf_alloc_mb+0x54>
    80000026:	84b2                	mv	s1,a2
  void *pa = alloc_page();
    80000028:	7b4000ef          	jal	800007dc <alloc_page>
    8000002c:	842a                	mv	s0,a0
  if (pa == 0) panic("uvmalloc mem alloc falied\n");
    8000002e:	c131                	beqz	a0,80000072 <elf_alloc_mb+0x60>
  memset((void *)pa, 0, PGSIZE);
    80000030:	6605                	lui	a2,0x1
    80000032:	4581                	li	a1,0
    80000034:	120040ef          	jal	80004154 <memset>
  user_vm_map((pagetable_t)msg->p->pagetable, elf_va, PGSIZE, (uint64)pa,
    80000038:	00893783          	ld	a5,8(s2)
    8000003c:	0087b903          	ld	s2,8(a5) # 1008 <elf_fpread-0x7fffeff8>
         prot_to_type(PROT_WRITE | PROT_READ | PROT_EXEC, 1));
    80000040:	4585                	li	a1,1
    80000042:	451d                	li	a0,7
    80000044:	7f0030ef          	jal	80003834 <prot_to_type>
  user_vm_map((pagetable_t)msg->p->pagetable, elf_va, PGSIZE, (uint64)pa,
    80000048:	0005071b          	sext.w	a4,a0
    8000004c:	86a2                	mv	a3,s0
    8000004e:	6605                	lui	a2,0x1
    80000050:	85a6                	mv	a1,s1
    80000052:	854a                	mv	a0,s2
    80000054:	22f030ef          	jal	80003a82 <user_vm_map>
}
    80000058:	8522                	mv	a0,s0
    8000005a:	60e2                	ld	ra,24(sp)
    8000005c:	6442                	ld	s0,16(sp)
    8000005e:	64a2                	ld	s1,8(sp)
    80000060:	6902                	ld	s2,0(sp)
    80000062:	6105                	addi	sp,sp,32
    80000064:	8082                	ret
  kassert(size < PGSIZE);
    80000066:	00007517          	auipc	a0,0x7
    8000006a:	19a50513          	addi	a0,a0,410 # 80007200 <rfs_i_ops+0x188>
    8000006e:	449040ef          	jal	80004cb6 <kassert_fail>
  if (pa == 0) panic("uvmalloc mem alloc falied\n");
    80000072:	00007517          	auipc	a0,0x7
    80000076:	1a650513          	addi	a0,a0,422 # 80007218 <rfs_i_ops+0x1a0>
    8000007a:	41d040ef          	jal	80004c96 <do_panic>

000000008000007e <parse_args>:

//
// returns the number (should be 1) of string(s) after PKE kernel in command line.
// and store the string(s) in arg_bug_msg.
//
static size_t parse_args(arg_buf *arg_bug_msg) {
    8000007e:	1141                	addi	sp,sp,-16
    80000080:	e406                	sd	ra,8(sp)
    80000082:	e022                	sd	s0,0(sp)
    80000084:	842a                	mv	s0,a0
  // HTIFSYS_getmainvars frontend call reads command arguments to (input) *arg_bug_msg
  long r = frontend_syscall(HTIFSYS_getmainvars, (uint64)arg_bug_msg,
    80000086:	4881                	li	a7,0
    80000088:	4801                	li	a6,0
    8000008a:	4781                	li	a5,0
    8000008c:	4701                	li	a4,0
    8000008e:	4681                	li	a3,0
    80000090:	20000613          	li	a2,512
    80000094:	85aa                	mv	a1,a0
    80000096:	7db00513          	li	a0,2011
    8000009a:	255040ef          	jal	80004aee <frontend_syscall>
      sizeof(*arg_bug_msg), 0, 0, 0, 0, 0);
  kassert(r == 0);
    8000009e:	e511                	bnez	a0,800000aa <parse_args+0x2c>

  size_t pk_argc = arg_bug_msg->buf[0];
    800000a0:	6008                	ld	a0,0(s0)
  uint64 *pk_argv = &arg_bug_msg->buf[1];
    800000a2:	00840613          	addi	a2,s0,8

  int arg = 1;  // skip the PKE OS kernel string, leave behind only the application name
  for (size_t i = 0; arg + i < pk_argc; i++)
    800000a6:	4781                	li	a5,0
    800000a8:	a839                	j	800000c6 <parse_args+0x48>
  kassert(r == 0);
    800000aa:	00007517          	auipc	a0,0x7
    800000ae:	18e50513          	addi	a0,a0,398 # 80007238 <rfs_i_ops+0x1c0>
    800000b2:	405040ef          	jal	80004cb6 <kassert_fail>
    arg_bug_msg->argv[i] = (char *)(uintptr_t)pk_argv[arg + i];
    800000b6:	00371693          	slli	a3,a4,0x3
    800000ba:	96b2                	add	a3,a3,a2
    800000bc:	6294                	ld	a3,0(a3)
    800000be:	078e                	slli	a5,a5,0x3
    800000c0:	97a2                	add	a5,a5,s0
    800000c2:	e394                	sd	a3,0(a5)
  for (size_t i = 0; arg + i < pk_argc; i++)
    800000c4:	87ba                	mv	a5,a4
    800000c6:	00178713          	addi	a4,a5,1
    800000ca:	fea766e3          	bltu	a4,a0,800000b6 <parse_args+0x38>

  //returns the number of strings after PKE kernel in command line
  return pk_argc - arg;
}
    800000ce:	157d                	addi	a0,a0,-1
    800000d0:	60a2                	ld	ra,8(sp)
    800000d2:	6402                	ld	s0,0(sp)
    800000d4:	0141                	addi	sp,sp,16
    800000d6:	8082                	ret

00000000800000d8 <elf_init>:
elf_status elf_init(elf_ctx *ctx, void *info) {
    800000d8:	1141                	addi	sp,sp,-16
    800000da:	e406                	sd	ra,8(sp)
    800000dc:	e022                	sd	s0,0(sp)
    800000de:	842a                	mv	s0,a0
  ctx->info = info;
    800000e0:	e10c                	sd	a1,0(a0)
  if (elf_fpread(ctx, &ctx->ehdr, sizeof(ctx->ehdr), 0) != sizeof(ctx->ehdr)) return EL_EIO;
    800000e2:	4681                	li	a3,0
    800000e4:	04000613          	li	a2,64
    800000e8:	00850593          	addi	a1,a0,8
    800000ec:	f15ff0ef          	jal	80000000 <elf_fpread>
    800000f0:	04000793          	li	a5,64
    800000f4:	00f51b63          	bne	a0,a5,8000010a <elf_init+0x32>
  if (ctx->ehdr.magic != ELF_MAGIC) return EL_NOTELF;
    800000f8:	4418                	lw	a4,8(s0)
    800000fa:	464c47b7          	lui	a5,0x464c4
    800000fe:	57f78793          	addi	a5,a5,1407 # 464c457f <elf_fpread-0x39b3ba81>
    80000102:	00f71963          	bne	a4,a5,80000114 <elf_init+0x3c>
  return EL_OK;
    80000106:	4501                	li	a0,0
    80000108:	a011                	j	8000010c <elf_init+0x34>
  if (elf_fpread(ctx, &ctx->ehdr, sizeof(ctx->ehdr), 0) != sizeof(ctx->ehdr)) return EL_EIO;
    8000010a:	4505                	li	a0,1
}
    8000010c:	60a2                	ld	ra,8(sp)
    8000010e:	6402                	ld	s0,0(sp)
    80000110:	0141                	addi	sp,sp,16
    80000112:	8082                	ret
  if (ctx->ehdr.magic != ELF_MAGIC) return EL_NOTELF;
    80000114:	450d                	li	a0,3
    80000116:	bfdd                	j	8000010c <elf_init+0x34>

0000000080000118 <elf_load>:
elf_status elf_load(elf_ctx *ctx) {
    80000118:	7159                	addi	sp,sp,-112
    8000011a:	f486                	sd	ra,104(sp)
    8000011c:	f0a2                	sd	s0,96(sp)
    8000011e:	eca6                	sd	s1,88(sp)
    80000120:	e8ca                	sd	s2,80(sp)
    80000122:	e4ce                	sd	s3,72(sp)
    80000124:	842a                	mv	s0,a0
  for (i = 0, off = ctx->ehdr.phoff; i < ctx->ehdr.phnum; i++, off += sizeof(ph_addr)) {
    80000126:	02852903          	lw	s2,40(a0)
    8000012a:	4481                	li	s1,0
    8000012c:	a891                	j	80000180 <elf_load+0x68>
    ((process*)(((elf_info*)(ctx->info))->p))->mapped_info[j].va = ph_addr.vaddr;
    8000012e:	601c                	ld	a5,0(s0)
    80000130:	679c                	ld	a5,8(a5)
    80000132:	6f98                	ld	a4,24(a5)
    80000134:	00459793          	slli	a5,a1,0x4
    80000138:	973e                	add	a4,a4,a5
    8000013a:	66e2                	ld	a3,24(sp)
    8000013c:	e314                	sd	a3,0(a4)
    ((process*)(((elf_info*)(ctx->info))->p))->mapped_info[j].npages = 1;
    8000013e:	6018                	ld	a4,0(s0)
    80000140:	6718                	ld	a4,8(a4)
    80000142:	6f18                	ld	a4,24(a4)
    80000144:	973e                	add	a4,a4,a5
    80000146:	4685                	li	a3,1
    80000148:	c714                	sw	a3,8(a4)
    if( ph_addr.flags == (SEGMENT_READABLE|SEGMENT_EXECUTABLE) ){
    8000014a:	4732                	lw	a4,12(sp)
    8000014c:	4695                	li	a3,5
    8000014e:	0ad70363          	beq	a4,a3,800001f4 <elf_load+0xdc>
    }else if ( ph_addr.flags == (SEGMENT_READABLE|SEGMENT_WRITABLE) ){
    80000152:	4699                	li	a3,6
    80000154:	0ad71d63          	bne	a4,a3,8000020e <elf_load+0xf6>
      ((process*)(((elf_info*)(ctx->info))->p))->mapped_info[j].seg_type = DATA_SEGMENT;
    80000158:	6018                	ld	a4,0(s0)
    8000015a:	6718                	ld	a4,8(a4)
    8000015c:	6f18                	ld	a4,24(a4)
    8000015e:	97ba                	add	a5,a5,a4
    80000160:	4715                	li	a4,5
    80000162:	c7d8                	sw	a4,12(a5)
      sprint( "DATA_SEGMENT added at mapped info offset:%d\n", j );
    80000164:	00007517          	auipc	a0,0x7
    80000168:	10c50513          	addi	a0,a0,268 # 80007270 <rfs_i_ops+0x1f8>
    8000016c:	287040ef          	jal	80004bf2 <sprint>
    ((process*)(((elf_info*)(ctx->info))->p))->total_mapped_region ++;
    80000170:	601c                	ld	a5,0(s0)
    80000172:	6798                	ld	a4,8(a5)
    80000174:	531c                	lw	a5,32(a4)
    80000176:	2785                	addiw	a5,a5,1
    80000178:	d31c                	sw	a5,32(a4)
  for (i = 0, off = ctx->ehdr.phoff; i < ctx->ehdr.phnum; i++, off += sizeof(ph_addr)) {
    8000017a:	2485                	addiw	s1,s1,1
    8000017c:	0389091b          	addiw	s2,s2,56
    80000180:	04045783          	lhu	a5,64(s0)
    80000184:	08f4dc63          	bge	s1,a5,8000021c <elf_load+0x104>
    if (elf_fpread(ctx, (void *)&ph_addr, sizeof(ph_addr), off) != sizeof(ph_addr)) return EL_EIO;
    80000188:	86ca                	mv	a3,s2
    8000018a:	03800613          	li	a2,56
    8000018e:	002c                	addi	a1,sp,8
    80000190:	8522                	mv	a0,s0
    80000192:	e6fff0ef          	jal	80000000 <elf_fpread>
    80000196:	03800793          	li	a5,56
    8000019a:	08f51a63          	bne	a0,a5,8000022e <elf_load+0x116>
    if (ph_addr.type != ELF_PROG_LOAD) continue;
    8000019e:	49a2                	lw	s3,8(sp)
    800001a0:	4785                	li	a5,1
    800001a2:	fcf99ce3          	bne	s3,a5,8000017a <elf_load+0x62>
    if (ph_addr.memsz < ph_addr.filesz) return EL_ERR;
    800001a6:	76c2                	ld	a3,48(sp)
    800001a8:	77a2                	ld	a5,40(sp)
    800001aa:	08f6e463          	bltu	a3,a5,80000232 <elf_load+0x11a>
    if (ph_addr.vaddr + ph_addr.memsz < ph_addr.vaddr) return EL_ERR;
    800001ae:	65e2                	ld	a1,24(sp)
    800001b0:	00b687b3          	add	a5,a3,a1
    800001b4:	08b7e163          	bltu	a5,a1,80000236 <elf_load+0x11e>
    void *dest = elf_alloc_mb(ctx, ph_addr.vaddr, ph_addr.vaddr, ph_addr.memsz);
    800001b8:	862e                	mv	a2,a1
    800001ba:	8522                	mv	a0,s0
    800001bc:	e57ff0ef          	jal	80000012 <elf_alloc_mb>
    800001c0:	85aa                	mv	a1,a0
    if (elf_fpread(ctx, dest, ph_addr.memsz, ph_addr.off) != ph_addr.memsz)
    800001c2:	66c2                	ld	a3,16(sp)
    800001c4:	7642                	ld	a2,48(sp)
    800001c6:	8522                	mv	a0,s0
    800001c8:	e39ff0ef          	jal	80000000 <elf_fpread>
    800001cc:	77c2                	ld	a5,48(sp)
    800001ce:	04f51863          	bne	a0,a5,8000021e <elf_load+0x106>
    for( j=0; j<PGSIZE/sizeof(mapped_region); j++ ) //seek the last mapped region
    800001d2:	4581                	li	a1,0
    800001d4:	0005879b          	sext.w	a5,a1
    800001d8:	0ff00713          	li	a4,255
    800001dc:	f4f769e3          	bltu	a4,a5,8000012e <elf_load+0x16>
      if( (process*)(((elf_info*)(ctx->info))->p)->mapped_info[j].va == 0x0 ) break;
    800001e0:	601c                	ld	a5,0(s0)
    800001e2:	679c                	ld	a5,8(a5)
    800001e4:	6f9c                	ld	a5,24(a5)
    800001e6:	00459713          	slli	a4,a1,0x4
    800001ea:	97ba                	add	a5,a5,a4
    800001ec:	639c                	ld	a5,0(a5)
    800001ee:	d3a1                	beqz	a5,8000012e <elf_load+0x16>
    for( j=0; j<PGSIZE/sizeof(mapped_region); j++ ) //seek the last mapped region
    800001f0:	2585                	addiw	a1,a1,1
    800001f2:	b7cd                	j	800001d4 <elf_load+0xbc>
      ((process*)(((elf_info*)(ctx->info))->p))->mapped_info[j].seg_type = CODE_SEGMENT;
    800001f4:	6018                	ld	a4,0(s0)
    800001f6:	6718                	ld	a4,8(a4)
    800001f8:	6f18                	ld	a4,24(a4)
    800001fa:	97ba                	add	a5,a5,a4
    800001fc:	4711                	li	a4,4
    800001fe:	c7d8                	sw	a4,12(a5)
      sprint( "CODE_SEGMENT added at mapped info offset:%d\n", j );
    80000200:	00007517          	auipc	a0,0x7
    80000204:	04050513          	addi	a0,a0,64 # 80007240 <rfs_i_ops+0x1c8>
    80000208:	1eb040ef          	jal	80004bf2 <sprint>
    8000020c:	b795                	j	80000170 <elf_load+0x58>
      panic( "unknown program segment encountered, segment flag:%d.\n", ph_addr.flags );
    8000020e:	85ba                	mv	a1,a4
    80000210:	00007517          	auipc	a0,0x7
    80000214:	09050513          	addi	a0,a0,144 # 800072a0 <rfs_i_ops+0x228>
    80000218:	27f040ef          	jal	80004c96 <do_panic>
  return EL_OK;
    8000021c:	4981                	li	s3,0
}
    8000021e:	854e                	mv	a0,s3
    80000220:	70a6                	ld	ra,104(sp)
    80000222:	7406                	ld	s0,96(sp)
    80000224:	64e6                	ld	s1,88(sp)
    80000226:	6946                	ld	s2,80(sp)
    80000228:	69a6                	ld	s3,72(sp)
    8000022a:	6165                	addi	sp,sp,112
    8000022c:	8082                	ret
    if (elf_fpread(ctx, (void *)&ph_addr, sizeof(ph_addr), off) != sizeof(ph_addr)) return EL_EIO;
    8000022e:	4985                	li	s3,1
    80000230:	b7fd                	j	8000021e <elf_load+0x106>
    if (ph_addr.memsz < ph_addr.filesz) return EL_ERR;
    80000232:	4991                	li	s3,4
    80000234:	b7ed                	j	8000021e <elf_load+0x106>
    if (ph_addr.vaddr + ph_addr.memsz < ph_addr.vaddr) return EL_ERR;
    80000236:	4991                	li	s3,4
    80000238:	b7dd                	j	8000021e <elf_load+0x106>

000000008000023a <load_bincode_from_host_elf>:

//
// load the elf of user application, by using the spike file interface.
//
void load_bincode_from_host_elf(process *p) {
    8000023a:	d9010113          	addi	sp,sp,-624
    8000023e:	26113423          	sd	ra,616(sp)
    80000242:	26813023          	sd	s0,608(sp)
    80000246:	842a                	mv	s0,a0
  arg_buf arg_bug_msg;

  // retrieve command line arguements
  size_t argc = parse_args(&arg_bug_msg);
    80000248:	1088                	addi	a0,sp,96
    8000024a:	e35ff0ef          	jal	8000007e <parse_args>
  if (!argc) panic("You need to specify the application program!\n");
    8000024e:	c135                	beqz	a0,800002b2 <load_bincode_from_host_elf+0x78>

  sprint("Application: %s\n", arg_bug_msg.argv[0]);
    80000250:	7586                	ld	a1,96(sp)
    80000252:	00007517          	auipc	a0,0x7
    80000256:	0b650513          	addi	a0,a0,182 # 80007308 <rfs_i_ops+0x290>
    8000025a:	199040ef          	jal	80004bf2 <sprint>
  //elf loading. elf_ctx is defined in kernel/elf.h, used to track the loading process.
  elf_ctx elfloader;
  // elf_info is defined above, used to tie the elf file and its corresponding process.
  elf_info info;

  info.f = spike_file_open(arg_bug_msg.argv[0], O_RDONLY, 0);
    8000025e:	4601                	li	a2,0
    80000260:	4581                	li	a1,0
    80000262:	7506                	ld	a0,96(sp)
    80000264:	346040ef          	jal	800045aa <spike_file_open>
    80000268:	e42a                	sd	a0,8(sp)
  info.p = p;
    8000026a:	e822                	sd	s0,16(sp)
  // IS_ERR_VALUE is a macro defined in spike_interface/spike_htif.h
  if (IS_ERR_VALUE(info.f)) panic("Fail on openning the input application program.\n");
    8000026c:	77fd                	lui	a5,0xfffff
    8000026e:	04f57863          	bgeu	a0,a5,800002be <load_bincode_from_host_elf+0x84>

  // init elfloader context. elf_init() is defined above.
  if (elf_init(&elfloader, &info) != EL_OK)
    80000272:	002c                	addi	a1,sp,8
    80000274:	0828                	addi	a0,sp,24
    80000276:	e63ff0ef          	jal	800000d8 <elf_init>
    8000027a:	e921                	bnez	a0,800002ca <load_bincode_from_host_elf+0x90>
    panic("fail to init elfloader.\n");

  // load elf. elf_load() is defined above.
  if (elf_load(&elfloader) != EL_OK) panic("Fail on loading elf.\n");
    8000027c:	0828                	addi	a0,sp,24
    8000027e:	e9bff0ef          	jal	80000118 <elf_load>
    80000282:	e931                	bnez	a0,800002d6 <load_bincode_from_host_elf+0x9c>

  // entry (virtual, also physical in lab1_x) address
  p->trapframe->epc = elfloader.ehdr.entry;
    80000284:	681c                	ld	a5,16(s0)
    80000286:	7762                	ld	a4,56(sp)
    80000288:	10e7b423          	sd	a4,264(a5) # fffffffffffff108 <_end+0xffffffff7ffee108>

  // close the host spike file
  spike_file_close( info.f );
    8000028c:	6522                	ld	a0,8(sp)
    8000028e:	1b8040ef          	jal	80004446 <spike_file_close>

  sprint("Application program entry point (virtual address): 0x%lx\n", p->trapframe->epc);
    80000292:	681c                	ld	a5,16(s0)
    80000294:	1087b583          	ld	a1,264(a5)
    80000298:	00007517          	auipc	a0,0x7
    8000029c:	0f850513          	addi	a0,a0,248 # 80007390 <rfs_i_ops+0x318>
    800002a0:	153040ef          	jal	80004bf2 <sprint>
}
    800002a4:	26813083          	ld	ra,616(sp)
    800002a8:	26013403          	ld	s0,608(sp)
    800002ac:	27010113          	addi	sp,sp,624
    800002b0:	8082                	ret
  if (!argc) panic("You need to specify the application program!\n");
    800002b2:	00007517          	auipc	a0,0x7
    800002b6:	02650513          	addi	a0,a0,38 # 800072d8 <rfs_i_ops+0x260>
    800002ba:	1dd040ef          	jal	80004c96 <do_panic>
  if (IS_ERR_VALUE(info.f)) panic("Fail on openning the input application program.\n");
    800002be:	00007517          	auipc	a0,0x7
    800002c2:	06250513          	addi	a0,a0,98 # 80007320 <rfs_i_ops+0x2a8>
    800002c6:	1d1040ef          	jal	80004c96 <do_panic>
    panic("fail to init elfloader.\n");
    800002ca:	00007517          	auipc	a0,0x7
    800002ce:	08e50513          	addi	a0,a0,142 # 80007358 <rfs_i_ops+0x2e0>
    800002d2:	1c5040ef          	jal	80004c96 <do_panic>
  if (elf_load(&elfloader) != EL_OK) panic("Fail on loading elf.\n");
    800002d6:	00007517          	auipc	a0,0x7
    800002da:	0a250513          	addi	a0,a0,162 # 80007378 <rfs_i_ops+0x300>
    800002de:	1b9040ef          	jal	80004c96 <do_panic>

00000000800002e2 <hostfs_write_back_vinode>:
  vinode->i_fs_info = NULL;
  vinode->i_ops = &hostfs_i_ops;
  return vinode;
}

int hostfs_write_back_vinode(struct vinode *vinode) { return 0; }
    800002e2:	4501                	li	a0,0
    800002e4:	8082                	ret

00000000800002e6 <hostfs_link>:
    return 0;
  return -1;
}

int hostfs_link(struct vinode *parent, struct dentry *sub_dentry,
                struct vinode *link_node) {
    800002e6:	1141                	addi	sp,sp,-16
    800002e8:	e406                	sd	ra,8(sp)
  panic("hostfs_link not implemented!\n");
    800002ea:	00007517          	auipc	a0,0x7
    800002ee:	0e650513          	addi	a0,a0,230 # 800073d0 <rfs_i_ops+0x358>
    800002f2:	1a5040ef          	jal	80004c96 <do_panic>

00000000800002f6 <hostfs_unlink>:
  return -1;
}

int hostfs_unlink(struct vinode *parent, struct dentry *sub_dentry, struct vinode *unlink_node) {
    800002f6:	1141                	addi	sp,sp,-16
    800002f8:	e406                	sd	ra,8(sp)
  panic("hostfs_unlink not implemented!\n");
    800002fa:	00007517          	auipc	a0,0x7
    800002fe:	0f650513          	addi	a0,a0,246 # 800073f0 <rfs_i_ops+0x378>
    80000302:	195040ef          	jal	80004c96 <do_panic>

0000000080000306 <hostfs_readdir>:
  return -1;
}

int hostfs_readdir(struct vinode *dir_vinode, struct dir *dir, int *offset) {
    80000306:	1141                	addi	sp,sp,-16
    80000308:	e406                	sd	ra,8(sp)
  panic("hostfs_readdir not implemented!\n");
    8000030a:	00007517          	auipc	a0,0x7
    8000030e:	10e50513          	addi	a0,a0,270 # 80007418 <rfs_i_ops+0x3a0>
    80000312:	185040ef          	jal	80004c96 <do_panic>

0000000080000316 <hostfs_mkdir>:
  return -1;
}

struct vinode *hostfs_mkdir(struct vinode *parent, struct dentry *sub_dentry) {
    80000316:	1141                	addi	sp,sp,-16
    80000318:	e406                	sd	ra,8(sp)
  panic("hostfs_mkdir not implemented!\n");
    8000031a:	00007517          	auipc	a0,0x7
    8000031e:	12650513          	addi	a0,a0,294 # 80007440 <rfs_i_ops+0x3c8>
    80000322:	175040ef          	jal	80004c96 <do_panic>

0000000080000326 <hostfs_read>:
                    int *offset) {
    80000326:	1101                	addi	sp,sp,-32
    80000328:	ec06                	sd	ra,24(sp)
    8000032a:	e822                	sd	s0,16(sp)
    8000032c:	e426                	sd	s1,8(sp)
    8000032e:	e04a                	sd	s2,0(sp)
    80000330:	84b6                	mv	s1,a3
  spike_file_t *pf = (spike_file_t *)f_inode->i_fs_info;
    80000332:	04053903          	ld	s2,64(a0)
  int read_len = spike_file_read(pf, r_buf, len);
    80000336:	854a                	mv	a0,s2
    80000338:	2aa040ef          	jal	800045e2 <spike_file_read>
    8000033c:	0005041b          	sext.w	s0,a0
  *offset = spike_file_lseek(pf, 0, 1);
    80000340:	4605                	li	a2,1
    80000342:	4581                	li	a1,0
    80000344:	854a                	mv	a0,s2
    80000346:	2bc040ef          	jal	80004602 <spike_file_lseek>
    8000034a:	c088                	sw	a0,0(s1)
}
    8000034c:	8522                	mv	a0,s0
    8000034e:	60e2                	ld	ra,24(sp)
    80000350:	6442                	ld	s0,16(sp)
    80000352:	64a2                	ld	s1,8(sp)
    80000354:	6902                	ld	s2,0(sp)
    80000356:	6105                	addi	sp,sp,32
    80000358:	8082                	ret

000000008000035a <hostfs_lseek>:
                  int *offset) {
    8000035a:	1141                	addi	sp,sp,-16
    8000035c:	e406                	sd	ra,8(sp)
    8000035e:	e022                	sd	s0,0(sp)
    80000360:	8436                	mv	s0,a3
  *offset = spike_file_lseek(f, new_offset, whence);
    80000362:	6128                	ld	a0,64(a0)
    80000364:	29e040ef          	jal	80004602 <spike_file_lseek>
    80000368:	2501                	sext.w	a0,a0
    8000036a:	c008                	sw	a0,0(s0)
  if (*offset >= 0)
    8000036c:	00054763          	bltz	a0,8000037a <hostfs_lseek+0x20>
    return 0;
    80000370:	4501                	li	a0,0
}
    80000372:	60a2                	ld	ra,8(sp)
    80000374:	6402                	ld	s0,0(sp)
    80000376:	0141                	addi	sp,sp,16
    80000378:	8082                	ret
  return -1;
    8000037a:	557d                	li	a0,-1
    8000037c:	bfdd                	j	80000372 <hostfs_lseek+0x18>

000000008000037e <hostfs_write>:
                     int *offset) {
    8000037e:	1101                	addi	sp,sp,-32
    80000380:	ec06                	sd	ra,24(sp)
    80000382:	e822                	sd	s0,16(sp)
    80000384:	e426                	sd	s1,8(sp)
    80000386:	e04a                	sd	s2,0(sp)
    80000388:	84b6                	mv	s1,a3
  spike_file_t *pf = (spike_file_t *)f_inode->i_fs_info;
    8000038a:	04053903          	ld	s2,64(a0)
  int write_len = spike_file_write(pf, w_buf, len);
    8000038e:	854a                	mv	a0,s2
    80000390:	12a040ef          	jal	800044ba <spike_file_write>
    80000394:	0005041b          	sext.w	s0,a0
  *offset = spike_file_lseek(pf, 0, 1);
    80000398:	4605                	li	a2,1
    8000039a:	4581                	li	a1,0
    8000039c:	854a                	mv	a0,s2
    8000039e:	264040ef          	jal	80004602 <spike_file_lseek>
    800003a2:	c088                	sw	a0,0(s1)
}
    800003a4:	8522                	mv	a0,s0
    800003a6:	60e2                	ld	ra,24(sp)
    800003a8:	6442                	ld	s0,16(sp)
    800003aa:	64a2                	ld	s1,8(sp)
    800003ac:	6902                	ld	s2,0(sp)
    800003ae:	6105                	addi	sp,sp,32
    800003b0:	8082                	ret

00000000800003b2 <hostfs_hook_close>:
}

//
// close a hostfs file.
//
int hostfs_hook_close(struct vinode *f_inode, struct dentry *dentry) {
    800003b2:	1141                	addi	sp,sp,-16
    800003b4:	e406                	sd	ra,8(sp)
  spike_file_t *f = (spike_file_t *)f_inode->i_fs_info;
  spike_file_close(f);
    800003b6:	6128                	ld	a0,64(a0)
    800003b8:	08e040ef          	jal	80004446 <spike_file_close>
  return 0;
}
    800003bc:	4501                	li	a0,0
    800003be:	60a2                	ld	ra,8(sp)
    800003c0:	0141                	addi	sp,sp,16
    800003c2:	8082                	ret

00000000800003c4 <register_hostfs>:
int register_hostfs() {
    800003c4:	1141                	addi	sp,sp,-16
    800003c6:	e406                	sd	ra,8(sp)
  struct file_system_type *fs_type = (struct file_system_type *)alloc_page();
    800003c8:	414000ef          	jal	800007dc <alloc_page>
  fs_type->type_num = HOSTFS_TYPE;
    800003cc:	4785                	li	a5,1
    800003ce:	c11c                	sw	a5,0(a0)
  fs_type->get_superblock = hostfs_get_superblock;
    800003d0:	00000797          	auipc	a5,0x0
    800003d4:	19078793          	addi	a5,a5,400 # 80000560 <hostfs_get_superblock>
    800003d8:	e51c                	sd	a5,8(a0)
  for (int i = 0; i < MAX_SUPPORTED_FS; i++) {
    800003da:	4781                	li	a5,0
    800003dc:	4725                	li	a4,9
    800003de:	02f74663          	blt	a4,a5,8000040a <register_hostfs+0x46>
    if (fs_list[i] == NULL) {
    800003e2:	00379693          	slli	a3,a5,0x3
    800003e6:	0000d717          	auipc	a4,0xd
    800003ea:	f1a70713          	addi	a4,a4,-230 # 8000d300 <fs_list>
    800003ee:	9736                	add	a4,a4,a3
    800003f0:	6318                	ld	a4,0(a4)
    800003f2:	c319                	beqz	a4,800003f8 <register_hostfs+0x34>
  for (int i = 0; i < MAX_SUPPORTED_FS; i++) {
    800003f4:	2785                	addiw	a5,a5,1
    800003f6:	b7dd                	j	800003dc <register_hostfs+0x18>
      fs_list[i] = fs_type;
    800003f8:	0000d717          	auipc	a4,0xd
    800003fc:	f0870713          	addi	a4,a4,-248 # 8000d300 <fs_list>
    80000400:	00d707b3          	add	a5,a4,a3
    80000404:	e388                	sd	a0,0(a5)
      return 0;
    80000406:	4501                	li	a0,0
    80000408:	a011                	j	8000040c <register_hostfs+0x48>
  return -1;
    8000040a:	557d                	li	a0,-1
}
    8000040c:	60a2                	ld	ra,8(sp)
    8000040e:	0141                	addi	sp,sp,16
    80000410:	8082                	ret

0000000080000412 <init_host_device>:
struct device *init_host_device(char *name) {
    80000412:	1101                	addi	sp,sp,-32
    80000414:	ec06                	sd	ra,24(sp)
    80000416:	e822                	sd	s0,16(sp)
    80000418:	e426                	sd	s1,8(sp)
    8000041a:	e04a                	sd	s2,0(sp)
    8000041c:	892a                	mv	s2,a0
  for (int i = 0; i < MAX_SUPPORTED_FS; i++) {
    8000041e:	4781                	li	a5,0
    80000420:	a011                	j	80000424 <init_host_device+0x12>
    80000422:	2785                	addiw	a5,a5,1
    80000424:	4725                	li	a4,9
    80000426:	02f74063          	blt	a4,a5,80000446 <init_host_device+0x34>
    if (fs_list[i] != NULL && fs_list[i]->type_num == HOSTFS_TYPE) {
    8000042a:	00379693          	slli	a3,a5,0x3
    8000042e:	0000d717          	auipc	a4,0xd
    80000432:	ed270713          	addi	a4,a4,-302 # 8000d300 <fs_list>
    80000436:	9736                	add	a4,a4,a3
    80000438:	6300                	ld	s0,0(a4)
    8000043a:	d465                	beqz	s0,80000422 <init_host_device+0x10>
    8000043c:	4014                	lw	a3,0(s0)
    8000043e:	4705                	li	a4,1
    80000440:	fee691e3          	bne	a3,a4,80000422 <init_host_device+0x10>
    80000444:	a011                	j	80000448 <init_host_device+0x36>
  struct file_system_type *fs_type = NULL;
    80000446:	4401                	li	s0,0
  if (!fs_type)
    80000448:	c80d                	beqz	s0,8000047a <init_host_device+0x68>
  struct device *device = (struct device *)alloc_page();
    8000044a:	392000ef          	jal	800007dc <alloc_page>
    8000044e:	84aa                	mv	s1,a0
  strcpy(device->dev_name, name);
    80000450:	85ca                	mv	a1,s2
    80000452:	571030ef          	jal	800041c2 <strcpy>
  device->dev_id = 0;
    80000456:	0204a023          	sw	zero,32(s1)
  device->fs_type = fs_type;
    8000045a:	f480                	sd	s0,40(s1)
  for (int i = 0; i < MAX_VFS_DEV; i++) {
    8000045c:	4781                	li	a5,0
    8000045e:	4725                	li	a4,9
    80000460:	02f74a63          	blt	a4,a5,80000494 <init_host_device+0x82>
    if (vfs_dev_list[i] == NULL) {
    80000464:	00379693          	slli	a3,a5,0x3
    80000468:	0000d717          	auipc	a4,0xd
    8000046c:	df870713          	addi	a4,a4,-520 # 8000d260 <vfs_dev_list>
    80000470:	9736                	add	a4,a4,a3
    80000472:	6318                	ld	a4,0(a4)
    80000474:	cb09                	beqz	a4,80000486 <init_host_device+0x74>
  for (int i = 0; i < MAX_VFS_DEV; i++) {
    80000476:	2785                	addiw	a5,a5,1
    80000478:	b7dd                	j	8000045e <init_host_device+0x4c>
    panic("init_host_device: No HOSTFS file system found!\n");
    8000047a:	00007517          	auipc	a0,0x7
    8000047e:	fe650513          	addi	a0,a0,-26 # 80007460 <rfs_i_ops+0x3e8>
    80000482:	015040ef          	jal	80004c96 <do_panic>
      vfs_dev_list[i] = device;
    80000486:	0000d717          	auipc	a4,0xd
    8000048a:	dda70713          	addi	a4,a4,-550 # 8000d260 <vfs_dev_list>
    8000048e:	00d707b3          	add	a5,a4,a3
    80000492:	e384                	sd	s1,0(a5)
}
    80000494:	8526                	mv	a0,s1
    80000496:	60e2                	ld	ra,24(sp)
    80000498:	6442                	ld	s0,16(sp)
    8000049a:	64a2                	ld	s1,8(sp)
    8000049c:	6902                	ld	s2,0(sp)
    8000049e:	6105                	addi	sp,sp,32
    800004a0:	8082                	ret

00000000800004a2 <path_backtrack>:
void path_backtrack(char *path, struct dentry *dentry) {
    800004a2:	1101                	addi	sp,sp,-32
    800004a4:	ec06                	sd	ra,24(sp)
    800004a6:	e426                	sd	s1,8(sp)
    800004a8:	84ae                	mv	s1,a1
  if (dentry->parent == NULL) {
    800004aa:	798c                	ld	a1,48(a1)
    800004ac:	c18d                	beqz	a1,800004ce <path_backtrack+0x2c>
    800004ae:	e822                	sd	s0,16(sp)
    800004b0:	842a                	mv	s0,a0
  path_backtrack(path, dentry->parent);
    800004b2:	ff1ff0ef          	jal	800004a2 <path_backtrack>
  strcat(path, "/");
    800004b6:	00007597          	auipc	a1,0x7
    800004ba:	fe258593          	addi	a1,a1,-30 # 80007498 <rfs_i_ops+0x420>
    800004be:	8522                	mv	a0,s0
    800004c0:	5b9030ef          	jal	80004278 <strcat>
  strcat(path, dentry->name);
    800004c4:	85a6                	mv	a1,s1
    800004c6:	8522                	mv	a0,s0
    800004c8:	5b1030ef          	jal	80004278 <strcat>
    800004cc:	6442                	ld	s0,16(sp)
}
    800004ce:	60e2                	ld	ra,24(sp)
    800004d0:	64a2                	ld	s1,8(sp)
    800004d2:	6105                	addi	sp,sp,32
    800004d4:	8082                	ret

00000000800004d6 <get_path_string>:
void get_path_string(char *path, struct dentry *dentry) {
    800004d6:	1101                	addi	sp,sp,-32
    800004d8:	ec06                	sd	ra,24(sp)
    800004da:	e822                	sd	s0,16(sp)
    800004dc:	e426                	sd	s1,8(sp)
    800004de:	842a                	mv	s0,a0
    800004e0:	84ae                	mv	s1,a1
  strcpy(path, H_ROOT_DIR);
    800004e2:	00007597          	auipc	a1,0x7
    800004e6:	fbe58593          	addi	a1,a1,-66 # 800074a0 <rfs_i_ops+0x428>
    800004ea:	4d9030ef          	jal	800041c2 <strcpy>
  path_backtrack(path, dentry);
    800004ee:	85a6                	mv	a1,s1
    800004f0:	8522                	mv	a0,s0
    800004f2:	fb1ff0ef          	jal	800004a2 <path_backtrack>
}
    800004f6:	60e2                	ld	ra,24(sp)
    800004f8:	6442                	ld	s0,16(sp)
    800004fa:	64a2                	ld	s1,8(sp)
    800004fc:	6105                	addi	sp,sp,32
    800004fe:	8082                	ret

0000000080000500 <hostfs_hook_open>:
  if (f_inode->i_fs_info != NULL) return 0;
    80000500:	613c                	ld	a5,64(a0)
    80000502:	c399                	beqz	a5,80000508 <hostfs_hook_open+0x8>
    80000504:	4501                	li	a0,0
}
    80000506:	8082                	ret
int hostfs_hook_open(struct vinode *f_inode, struct dentry *f_dentry) {
    80000508:	7179                	addi	sp,sp,-48
    8000050a:	f406                	sd	ra,40(sp)
    8000050c:	f022                	sd	s0,32(sp)
    8000050e:	842a                	mv	s0,a0
  get_path_string(path, f_dentry);
    80000510:	850a                	mv	a0,sp
    80000512:	fc5ff0ef          	jal	800004d6 <get_path_string>
  spike_file_t *f = spike_file_open(path, O_RDWR, 0);
    80000516:	4601                	li	a2,0
    80000518:	4589                	li	a1,2
    8000051a:	850a                	mv	a0,sp
    8000051c:	08e040ef          	jal	800045aa <spike_file_open>
  if ((int64)f < 0) {
    80000520:	00054863          	bltz	a0,80000530 <hostfs_hook_open+0x30>
  f_inode->i_fs_info = f;
    80000524:	e028                	sd	a0,64(s0)
  return 0;
    80000526:	4501                	li	a0,0
}
    80000528:	70a2                	ld	ra,40(sp)
    8000052a:	7402                	ld	s0,32(sp)
    8000052c:	6145                	addi	sp,sp,48
    8000052e:	8082                	ret
    sprint("hostfs_hook_open cannot open the given file.\n");
    80000530:	00007517          	auipc	a0,0x7
    80000534:	f8050513          	addi	a0,a0,-128 # 800074b0 <rfs_i_ops+0x438>
    80000538:	6ba040ef          	jal	80004bf2 <sprint>
    return -1;
    8000053c:	557d                	li	a0,-1
    8000053e:	b7ed                	j	80000528 <hostfs_hook_open+0x28>

0000000080000540 <hostfs_alloc_vinode>:
struct vinode *hostfs_alloc_vinode(struct super_block *sb) {
    80000540:	1141                	addi	sp,sp,-16
    80000542:	e406                	sd	ra,8(sp)
  struct vinode *vinode = default_alloc_vinode(sb);
    80000544:	2c6030ef          	jal	8000380a <default_alloc_vinode>
  vinode->inum = -1; 
    80000548:	577d                	li	a4,-1
    8000054a:	c118                	sw	a4,0(a0)
  vinode->i_fs_info = NULL;
    8000054c:	04053023          	sd	zero,64(a0)
  vinode->i_ops = &hostfs_i_ops;
    80000550:	00007717          	auipc	a4,0x7
    80000554:	ab070713          	addi	a4,a4,-1360 # 80007000 <hostfs_i_ops>
    80000558:	e938                	sd	a4,80(a0)
}
    8000055a:	60a2                	ld	ra,8(sp)
    8000055c:	0141                	addi	sp,sp,16
    8000055e:	8082                	ret

0000000080000560 <hostfs_get_superblock>:

/**** vfs-hostfs file system type interface functions ****/
struct super_block *hostfs_get_superblock(struct device *dev) {
    80000560:	1101                	addi	sp,sp,-32
    80000562:	ec06                	sd	ra,24(sp)
    80000564:	e822                	sd	s0,16(sp)
    80000566:	e426                	sd	s1,8(sp)
    80000568:	84aa                	mv	s1,a0
  // set the data for the vfs super block
  struct super_block *sb = alloc_page();
    8000056a:	272000ef          	jal	800007dc <alloc_page>
    8000056e:	842a                	mv	s0,a0
  sb->s_dev = dev;
    80000570:	ed04                	sd	s1,24(a0)

  struct vinode *root_inode = hostfs_alloc_vinode(sb);
    80000572:	fcfff0ef          	jal	80000540 <hostfs_alloc_vinode>
    80000576:	85aa                	mv	a1,a0
  root_inode->type = H_DIR;
    80000578:	4785                	li	a5,1
    8000057a:	c55c                	sw	a5,12(a0)

  struct dentry *root_dentry = alloc_vfs_dentry("/", root_inode, NULL);
    8000057c:	4601                	li	a2,0
    8000057e:	00007517          	auipc	a0,0x7
    80000582:	f1a50513          	addi	a0,a0,-230 # 80007498 <rfs_i_ops+0x420>
    80000586:	2b5020ef          	jal	8000303a <alloc_vfs_dentry>
  sb->s_root = root_dentry;
    8000058a:	e808                	sd	a0,16(s0)

  return sb;
}
    8000058c:	8522                	mv	a0,s0
    8000058e:	60e2                	ld	ra,24(sp)
    80000590:	6442                	ld	s0,16(sp)
    80000592:	64a2                	ld	s1,8(sp)
    80000594:	6105                	addi	sp,sp,32
    80000596:	8082                	ret

0000000080000598 <hostfs_update_vinode>:
int hostfs_update_vinode(struct vinode *vinode) {
    80000598:	7119                	addi	sp,sp,-128
    8000059a:	fc86                	sd	ra,120(sp)
    8000059c:	f8a2                	sd	s0,112(sp)
    8000059e:	842a                	mv	s0,a0
  spike_file_t *f = vinode->i_fs_info;
    800005a0:	6128                	ld	a0,64(a0)
  if ((int64)f < 0) {  // is a direntry
    800005a2:	04054063          	bltz	a0,800005e2 <hostfs_update_vinode+0x4a>
  spike_file_stat(f, &stat);
    800005a6:	002c                	addi	a1,sp,8
    800005a8:	62d030ef          	jal	800043d4 <spike_file_stat>
  vinode->inum = stat.st_ino;
    800005ac:	00a15783          	lhu	a5,10(sp)
    800005b0:	c01c                	sw	a5,0(s0)
  vinode->size = stat.st_size;
    800005b2:	67e2                	ld	a5,24(sp)
    800005b4:	c41c                	sw	a5,8(s0)
  vinode->nlinks = stat.st_nlink;
    800005b6:	01015783          	lhu	a5,16(sp)
    800005ba:	c81c                	sw	a5,16(s0)
  vinode->blocks = stat.st_blocks;
    800005bc:	67e6                	ld	a5,88(sp)
    800005be:	c85c                	sw	a5,20(s0)
  if (S_ISDIR(stat.st_mode)) {
    800005c0:	47b2                	lw	a5,12(sp)
    800005c2:	673d                	lui	a4,0xf
    800005c4:	8ff9                	and	a5,a5,a4
    800005c6:	2781                	sext.w	a5,a5
    800005c8:	6711                	lui	a4,0x4
    800005ca:	02e78063          	beq	a5,a4,800005ea <hostfs_update_vinode+0x52>
  } else if (S_ISREG(stat.st_mode)) {
    800005ce:	6721                	lui	a4,0x8
    800005d0:	02e79163          	bne	a5,a4,800005f2 <hostfs_update_vinode+0x5a>
    vinode->type = H_FILE;
    800005d4:	00042623          	sw	zero,12(s0)
  return 0;
    800005d8:	4501                	li	a0,0
}
    800005da:	70e6                	ld	ra,120(sp)
    800005dc:	7446                	ld	s0,112(sp)
    800005de:	6109                	addi	sp,sp,128
    800005e0:	8082                	ret
    vinode->type = H_DIR;
    800005e2:	4785                	li	a5,1
    800005e4:	c45c                	sw	a5,12(s0)
    return -1;
    800005e6:	557d                	li	a0,-1
    800005e8:	bfcd                	j	800005da <hostfs_update_vinode+0x42>
    vinode->type = H_DIR;
    800005ea:	4785                	li	a5,1
    800005ec:	c45c                	sw	a5,12(s0)
  return 0;
    800005ee:	4501                	li	a0,0
    800005f0:	b7ed                	j	800005da <hostfs_update_vinode+0x42>
    sprint("hostfs_lookup:unknown file type!");
    800005f2:	00007517          	auipc	a0,0x7
    800005f6:	eee50513          	addi	a0,a0,-274 # 800074e0 <rfs_i_ops+0x468>
    800005fa:	5f8040ef          	jal	80004bf2 <sprint>
    return -1;
    800005fe:	557d                	li	a0,-1
    80000600:	bfe9                	j	800005da <hostfs_update_vinode+0x42>

0000000080000602 <hostfs_lookup>:
struct vinode *hostfs_lookup(struct vinode *parent, struct dentry *sub_dentry) {
    80000602:	7139                	addi	sp,sp,-64
    80000604:	fc06                	sd	ra,56(sp)
    80000606:	f822                	sd	s0,48(sp)
    80000608:	f426                	sd	s1,40(sp)
    8000060a:	842a                	mv	s0,a0
  get_path_string(path, sub_dentry);
    8000060c:	850a                	mv	a0,sp
    8000060e:	ec9ff0ef          	jal	800004d6 <get_path_string>
  spike_file_t *f = spike_file_open(path, O_RDWR, 0);
    80000612:	4601                	li	a2,0
    80000614:	4589                	li	a1,2
    80000616:	850a                	mv	a0,sp
    80000618:	793030ef          	jal	800045aa <spike_file_open>
    8000061c:	84aa                	mv	s1,a0
  struct vinode *child_inode = hostfs_alloc_vinode(parent->sb);
    8000061e:	6428                	ld	a0,72(s0)
    80000620:	f21ff0ef          	jal	80000540 <hostfs_alloc_vinode>
    80000624:	842a                	mv	s0,a0
  child_inode->i_fs_info = f;
    80000626:	e124                	sd	s1,64(a0)
  hostfs_update_vinode(child_inode);
    80000628:	f71ff0ef          	jal	80000598 <hostfs_update_vinode>
  child_inode->ref = 0;
    8000062c:	00042223          	sw	zero,4(s0)
}
    80000630:	8522                	mv	a0,s0
    80000632:	70e2                	ld	ra,56(sp)
    80000634:	7442                	ld	s0,48(sp)
    80000636:	74a2                	ld	s1,40(sp)
    80000638:	6121                	addi	sp,sp,64
    8000063a:	8082                	ret

000000008000063c <hostfs_create>:
struct vinode *hostfs_create(struct vinode *parent, struct dentry *sub_dentry) {
    8000063c:	7139                	addi	sp,sp,-64
    8000063e:	fc06                	sd	ra,56(sp)
    80000640:	f822                	sd	s0,48(sp)
    80000642:	842a                	mv	s0,a0
  get_path_string(path, sub_dentry);
    80000644:	850a                	mv	a0,sp
    80000646:	e91ff0ef          	jal	800004d6 <get_path_string>
  spike_file_t *f = spike_file_open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    8000064a:	18000613          	li	a2,384
    8000064e:	04200593          	li	a1,66
    80000652:	850a                	mv	a0,sp
    80000654:	757030ef          	jal	800045aa <spike_file_open>
  if ((int64)f < 0) {
    80000658:	02054463          	bltz	a0,80000680 <hostfs_create+0x44>
    8000065c:	f426                	sd	s1,40(sp)
    8000065e:	84aa                	mv	s1,a0
  struct vinode *new_inode = hostfs_alloc_vinode(parent->sb);
    80000660:	6428                	ld	a0,72(s0)
    80000662:	edfff0ef          	jal	80000540 <hostfs_alloc_vinode>
    80000666:	842a                	mv	s0,a0
  new_inode->i_fs_info = f;
    80000668:	e124                	sd	s1,64(a0)
  if (hostfs_update_vinode(new_inode) != 0) return NULL;
    8000066a:	f2fff0ef          	jal	80000598 <hostfs_update_vinode>
    8000066e:	e10d                	bnez	a0,80000690 <hostfs_create+0x54>
  new_inode->ref = 0;
    80000670:	00042223          	sw	zero,4(s0)
  return new_inode;
    80000674:	74a2                	ld	s1,40(sp)
}
    80000676:	8522                	mv	a0,s0
    80000678:	70e2                	ld	ra,56(sp)
    8000067a:	7442                	ld	s0,48(sp)
    8000067c:	6121                	addi	sp,sp,64
    8000067e:	8082                	ret
    sprint("hostfs_create cannot create the given file.\n");
    80000680:	00007517          	auipc	a0,0x7
    80000684:	e8850513          	addi	a0,a0,-376 # 80007508 <rfs_i_ops+0x490>
    80000688:	56a040ef          	jal	80004bf2 <sprint>
    return NULL;
    8000068c:	4401                	li	s0,0
    8000068e:	b7e5                	j	80000676 <hostfs_create+0x3a>
  if (hostfs_update_vinode(new_inode) != 0) return NULL;
    80000690:	4401                	li	s0,0
    80000692:	74a2                	ld	s1,40(sp)
    80000694:	b7cd                	j	80000676 <hostfs_create+0x3a>

0000000080000696 <enable_paging>:
//
// turn on paging. added @lab2_1
//
void enable_paging() {
  // write the pointer to kernel page (table) directory into the CSR of "satp".
  write_csr(satp, MAKE_SATP(g_kernel_pagetable));
    80000696:	00010797          	auipc	a5,0x10
    8000069a:	ab27b783          	ld	a5,-1358(a5) # 80010148 <g_kernel_pagetable>
    8000069e:	83b1                	srli	a5,a5,0xc
    800006a0:	577d                	li	a4,-1
    800006a2:	177e                	slli	a4,a4,0x3f
    800006a4:	8fd9                	or	a5,a5,a4
    800006a6:	18079073          	csrw	satp,a5
  /* 232 */ uint64 t5;
  /* 240 */ uint64 t6;
}riscv_regs;

// following lines are added @lab2_1
static inline void flush_tlb(void) { asm volatile("sfence.vma zero, zero"); }
    800006aa:	12000073          	sfence.vma

  // refresh tlb to invalidate its content.
  flush_tlb();
}
    800006ae:	8082                	ret

00000000800006b0 <load_user_program>:

//
// load the elf, and construct a "process" (with only a trapframe).
// load_bincode_from_host_elf is defined in elf.c
//
process* load_user_program() {
    800006b0:	1141                	addi	sp,sp,-16
    800006b2:	e406                	sd	ra,8(sp)
    800006b4:	e022                	sd	s0,0(sp)
  process* proc;

  proc = alloc_process();
    800006b6:	3dc000ef          	jal	80000a92 <alloc_process>
    800006ba:	842a                	mv	s0,a0
  sprint("User application is loading.\n");
    800006bc:	00007517          	auipc	a0,0x7
    800006c0:	e7c50513          	addi	a0,a0,-388 # 80007538 <rfs_i_ops+0x4c0>
    800006c4:	52e040ef          	jal	80004bf2 <sprint>

  load_bincode_from_host_elf(proc);
    800006c8:	8522                	mv	a0,s0
    800006ca:	b71ff0ef          	jal	8000023a <load_bincode_from_host_elf>
  return proc;
}
    800006ce:	8522                	mv	a0,s0
    800006d0:	60a2                	ld	ra,8(sp)
    800006d2:	6402                	ld	s0,0(sp)
    800006d4:	0141                	addi	sp,sp,16
    800006d6:	8082                	ret

00000000800006d8 <s_start>:

//
// s_start: S-mode entry point of riscv-pke OS kernel.
//
int s_start(void) {
    800006d8:	1141                	addi	sp,sp,-16
    800006da:	e406                	sd	ra,8(sp)
  sprint("Enter supervisor mode...\n");
    800006dc:	00007517          	auipc	a0,0x7
    800006e0:	e7c50513          	addi	a0,a0,-388 # 80007558 <rfs_i_ops+0x4e0>
    800006e4:	50e040ef          	jal	80004bf2 <sprint>
  // in the beginning, we use Bare mode (direct) memory mapping as in lab1.
  // but now, we are going to switch to the paging mode @lab2_1.
  // note, the code still works in Bare mode when calling pmm_init() and kern_vm_init().
  write_csr(satp, 0);
    800006e8:	18005073          	csrwi	satp,0

  // init phisical memory manager
  pmm_init();
    800006ec:	106000ef          	jal	800007f2 <pmm_init>

  // build the kernel page table
  kern_vm_init();
    800006f0:	2dc030ef          	jal	800039cc <kern_vm_init>

  // now, switch to paging mode by turning on paging (SV39)
  enable_paging();
    800006f4:	fa3ff0ef          	jal	80000696 <enable_paging>
  // the code now formally works in paging mode, meaning the page table is now in use.
  sprint("kernel page table is on \n");
    800006f8:	00007517          	auipc	a0,0x7
    800006fc:	e8050513          	addi	a0,a0,-384 # 80007578 <rfs_i_ops+0x500>
    80000700:	4f2040ef          	jal	80004bf2 <sprint>

  // added @lab3_1
  init_proc_pool();
    80000704:	34a000ef          	jal	80000a4e <init_proc_pool>

  // init file system, added @lab4_1
  fs_init();
    80000708:	043000ef          	jal	80000f4a <fs_init>

  sprint("Switch to user mode...\n");
    8000070c:	00007517          	auipc	a0,0x7
    80000710:	e8c50513          	addi	a0,a0,-372 # 80007598 <rfs_i_ops+0x520>
    80000714:	4de040ef          	jal	80004bf2 <sprint>
  // the application code (elf) is first loaded into memory, and then put into execution
  // added @lab3_1
  insert_to_ready_queue( load_user_program() );
    80000718:	f99ff0ef          	jal	800006b0 <load_user_program>
    8000071c:	639010ef          	jal	80002554 <insert_to_ready_queue>
  schedule();
    80000720:	693010ef          	jal	800025b2 <schedule>

  // we should never reach here.
  return 0;
}
    80000724:	4501                	li	a0,0
    80000726:	60a2                	ld	ra,8(sp)
    80000728:	0141                	addi	sp,sp,16
    8000072a:	8082                	ret

000000008000072c <free_page>:
}

//
// place a physical page at *pa to the free list of g_free_mem_list (to reclaim the page)
//
void free_page(void *pa) {
    8000072c:	85aa                	mv	a1,a0
  if (((uint64)pa % PGSIZE) != 0 || (uint64)pa < free_mem_start_addr || (uint64)pa >= free_mem_end_addr)
    8000072e:	872a                	mv	a4,a0
    80000730:	6785                	lui	a5,0x1
    80000732:	17fd                	addi	a5,a5,-1 # fff <elf_fpread-0x7ffff001>
    80000734:	8fe9                	and	a5,a5,a0
    80000736:	ef89                	bnez	a5,80000750 <free_page+0x24>
    80000738:	00010797          	auipc	a5,0x10
    8000073c:	9e87b783          	ld	a5,-1560(a5) # 80010120 <free_mem_start_addr>
    80000740:	00f56863          	bltu	a0,a5,80000750 <free_page+0x24>
    80000744:	00010797          	auipc	a5,0x10
    80000748:	9d47b783          	ld	a5,-1580(a5) # 80010118 <free_mem_end_addr>
    8000074c:	04f56663          	bltu	a0,a5,80000798 <free_page+0x6c>
void free_page(void *pa) {
    80000750:	1141                	addi	sp,sp,-16
    80000752:	e406                	sd	ra,8(sp)
  {
    if ((uint64)pa < free_mem_start_addr )
    80000754:	00010617          	auipc	a2,0x10
    80000758:	9cc63603          	ld	a2,-1588(a2) # 80010120 <free_mem_start_addr>
    8000075c:	02c76163          	bltu	a4,a2,8000077e <free_page+0x52>
    {
      sprint("free_page 0x%lx < free_mem_start_addr 0x%lx \n", pa, free_mem_start_addr);
      return; 
    }else if ( (uint64)pa >= free_mem_end_addr){
    80000760:	00010617          	auipc	a2,0x10
    80000764:	9b863603          	ld	a2,-1608(a2) # 80010118 <free_mem_end_addr>
    80000768:	02c76263          	bltu	a4,a2,8000078c <free_page+0x60>
      sprint("free_page 0x%lx >= free_mem_end_addr 0x%lx \n", pa, free_mem_end_addr);
    8000076c:	00007517          	auipc	a0,0x7
    80000770:	e7450513          	addi	a0,a0,-396 # 800075e0 <rfs_i_ops+0x568>
    80000774:	47e040ef          	jal	80004bf2 <sprint>

  // insert a physical page to g_free_mem_list
  list_node *n = (list_node *)pa;
  n->next = g_free_mem_list.next;
  g_free_mem_list.next = n;
}
    80000778:	60a2                	ld	ra,8(sp)
    8000077a:	0141                	addi	sp,sp,16
    8000077c:	8082                	ret
      sprint("free_page 0x%lx < free_mem_start_addr 0x%lx \n", pa, free_mem_start_addr);
    8000077e:	00007517          	auipc	a0,0x7
    80000782:	e3250513          	addi	a0,a0,-462 # 800075b0 <rfs_i_ops+0x538>
    80000786:	46c040ef          	jal	80004bf2 <sprint>
      return; 
    8000078a:	b7fd                	j	80000778 <free_page+0x4c>
    panic("free_page 0x%lx \n", pa);
    8000078c:	00007517          	auipc	a0,0x7
    80000790:	e8450513          	addi	a0,a0,-380 # 80007610 <rfs_i_ops+0x598>
    80000794:	502040ef          	jal	80004c96 <do_panic>
  n->next = g_free_mem_list.next;
    80000798:	00010797          	auipc	a5,0x10
    8000079c:	97878793          	addi	a5,a5,-1672 # 80010110 <g_free_mem_list>
    800007a0:	6398                	ld	a4,0(a5)
    800007a2:	e118                	sd	a4,0(a0)
  g_free_mem_list.next = n;
    800007a4:	e388                	sd	a0,0(a5)
    800007a6:	8082                	ret

00000000800007a8 <create_freepage_list>:
static void create_freepage_list(uint64 start, uint64 end) {
    800007a8:	1101                	addi	sp,sp,-32
    800007aa:	ec06                	sd	ra,24(sp)
    800007ac:	e822                	sd	s0,16(sp)
    800007ae:	e426                	sd	s1,8(sp)
    800007b0:	84ae                	mv	s1,a1
  g_free_mem_list.next = 0;
    800007b2:	00010797          	auipc	a5,0x10
    800007b6:	9407bf23          	sd	zero,-1698(a5) # 80010110 <g_free_mem_list>
  for (uint64 p = ROUNDUP(start, PGSIZE); p + PGSIZE < end; p += PGSIZE)
    800007ba:	157d                	addi	a0,a0,-1
    800007bc:	8131                	srli	a0,a0,0xc
    800007be:	0505                	addi	a0,a0,1
    800007c0:	0532                	slli	a0,a0,0xc
    800007c2:	a021                	j	800007ca <create_freepage_list+0x22>
    free_page( (void *)p );
    800007c4:	f69ff0ef          	jal	8000072c <free_page>
  for (uint64 p = ROUNDUP(start, PGSIZE); p + PGSIZE < end; p += PGSIZE)
    800007c8:	8522                	mv	a0,s0
    800007ca:	6405                	lui	s0,0x1
    800007cc:	942a                	add	s0,s0,a0
    800007ce:	fe946be3          	bltu	s0,s1,800007c4 <create_freepage_list+0x1c>
}
    800007d2:	60e2                	ld	ra,24(sp)
    800007d4:	6442                	ld	s0,16(sp)
    800007d6:	64a2                	ld	s1,8(sp)
    800007d8:	6105                	addi	sp,sp,32
    800007da:	8082                	ret

00000000800007dc <alloc_page>:
//
// takes the first free page from g_free_mem_list, and returns (allocates) it.
// Allocates only ONE page!
//
void *alloc_page(void) {
  list_node *n = g_free_mem_list.next;
    800007dc:	00010517          	auipc	a0,0x10
    800007e0:	93453503          	ld	a0,-1740(a0) # 80010110 <g_free_mem_list>
  if (n) g_free_mem_list.next = n->next;
    800007e4:	c511                	beqz	a0,800007f0 <alloc_page+0x14>
    800007e6:	611c                	ld	a5,0(a0)
    800007e8:	00010717          	auipc	a4,0x10
    800007ec:	92f73423          	sd	a5,-1752(a4) # 80010110 <g_free_mem_list>

  return (void *)n;
}
    800007f0:	8082                	ret

00000000800007f2 <pmm_init>:

//
// pmm_init() establishes the list of free physical pages according to available
// physical memory space.
//
void pmm_init() {
    800007f2:	1141                	addi	sp,sp,-16
    800007f4:	e406                	sd	ra,8(sp)
    800007f6:	e022                	sd	s0,0(sp)
  // start of kernel program segment
  uint64 g_kernel_start = KERN_BASE;
  uint64 g_kernel_end = (uint64)&_end;

  uint64 pke_kernel_size = g_kernel_end - g_kernel_start;
    800007f8:	80011417          	auipc	s0,0x80011
    800007fc:	80840413          	addi	s0,s0,-2040 # 11000 <elf_fpread-0x7ffef000>
  sprint("PKE kernel start 0x%lx, PKE kernel end: 0x%lx, PKE kernel size: 0x%lx .\n",
    80000800:	86a2                	mv	a3,s0
    80000802:	00010617          	auipc	a2,0x10
    80000806:	7fe60613          	addi	a2,a2,2046 # 80011000 <_end>
    8000080a:	4585                	li	a1,1
    8000080c:	05fe                	slli	a1,a1,0x1f
    8000080e:	00007517          	auipc	a0,0x7
    80000812:	e1a50513          	addi	a0,a0,-486 # 80007628 <rfs_i_ops+0x5b0>
    80000816:	3dc040ef          	jal	80004bf2 <sprint>
    g_kernel_start, g_kernel_end, pke_kernel_size);

  // free memory starts from the end of PKE kernel and must be page-aligined
  free_mem_start_addr = ROUNDUP(g_kernel_end , PGSIZE);
    8000081a:	00010597          	auipc	a1,0x10
    8000081e:	7e558593          	addi	a1,a1,2021 # 80010fff <g_mem_size+0xe97>
    80000822:	81b1                	srli	a1,a1,0xc
    80000824:	0585                	addi	a1,a1,1
    80000826:	05b2                	slli	a1,a1,0xc
    80000828:	00010797          	auipc	a5,0x10
    8000082c:	8eb7bc23          	sd	a1,-1800(a5) # 80010120 <free_mem_start_addr>

  // recompute g_mem_size to limit the physical memory space that our riscv-pke kernel
  // needs to manage
  g_mem_size = MIN(PKE_MAX_ALLOWABLE_RAM, g_mem_size);
    80000830:	00010797          	auipc	a5,0x10
    80000834:	9387b783          	ld	a5,-1736(a5) # 80010168 <g_mem_size>
    80000838:	08000737          	lui	a4,0x8000
    8000083c:	00f77463          	bgeu	a4,a5,80000844 <pmm_init+0x52>
    80000840:	080007b7          	lui	a5,0x8000
    80000844:	00010717          	auipc	a4,0x10
    80000848:	92f73223          	sd	a5,-1756(a4) # 80010168 <g_mem_size>
  if( g_mem_size < pke_kernel_size )
    8000084c:	0487e663          	bltu	a5,s0,80000898 <pmm_init+0xa6>
    panic( "Error when recomputing physical memory size (g_mem_size).\n" );

  free_mem_end_addr = g_mem_size + DRAM_BASE;
    80000850:	4705                	li	a4,1
    80000852:	077e                	slli	a4,a4,0x1f
    80000854:	973e                	add	a4,a4,a5
    80000856:	00010417          	auipc	s0,0x10
    8000085a:	8c240413          	addi	s0,s0,-1854 # 80010118 <free_mem_end_addr>
    8000085e:	e018                	sd	a4,0(s0)
  sprint("free physical memory address: [0x%lx, 0x%lx] \n", free_mem_start_addr,
    80000860:	80000637          	lui	a2,0x80000
    80000864:	fff64613          	not	a2,a2
    80000868:	963e                	add	a2,a2,a5
    8000086a:	00007517          	auipc	a0,0x7
    8000086e:	e4e50513          	addi	a0,a0,-434 # 800076b8 <rfs_i_ops+0x640>
    80000872:	380040ef          	jal	80004bf2 <sprint>
    free_mem_end_addr - 1);

  sprint("kernel memory manager is initializing ...\n");
    80000876:	00007517          	auipc	a0,0x7
    8000087a:	e7250513          	addi	a0,a0,-398 # 800076e8 <rfs_i_ops+0x670>
    8000087e:	374040ef          	jal	80004bf2 <sprint>
  // create the list of free pages
  create_freepage_list(free_mem_start_addr, free_mem_end_addr);
    80000882:	600c                	ld	a1,0(s0)
    80000884:	00010517          	auipc	a0,0x10
    80000888:	89c53503          	ld	a0,-1892(a0) # 80010120 <free_mem_start_addr>
    8000088c:	f1dff0ef          	jal	800007a8 <create_freepage_list>
}
    80000890:	60a2                	ld	ra,8(sp)
    80000892:	6402                	ld	s0,0(sp)
    80000894:	0141                	addi	sp,sp,16
    80000896:	8082                	ret
    panic( "Error when recomputing physical memory size (g_mem_size).\n" );
    80000898:	00007517          	auipc	a0,0x7
    8000089c:	de050513          	addi	a0,a0,-544 # 80007678 <rfs_i_ops+0x600>
    800008a0:	3f6040ef          	jal	80004c96 <do_panic>

00000000800008a4 <exec_clean_pagetable>:
  return child->pid;
}


// 根据alloc_process函数改写
static void exec_clean_pagetable(pagetable_t page_dir) { // comment: pagetable_t是uint64* 其加法遵循指针加法
    800008a4:	711d                	addi	sp,sp,-96
    800008a6:	ec86                	sd	ra,88(sp)
    800008a8:	e8a2                	sd	s0,80(sp)
    800008aa:	e4a6                	sd	s1,72(sp)
    800008ac:	e0ca                	sd	s2,64(sp)
    800008ae:	fc4e                	sd	s3,56(sp)
    800008b0:	f852                	sd	s4,48(sp)
    800008b2:	f456                	sd	s5,40(sp)
    800008b4:	f05a                	sd	s6,32(sp)
    800008b6:	ec5e                	sd	s7,24(sp)
    800008b8:	e862                	sd	s8,16(sp)
    800008ba:	e466                	sd	s9,8(sp)
    800008bc:	8baa                	mv	s7,a0
     // 三级页表
    int cnt = PGSIZE / sizeof(pte_t); // pte_t是int型
    sprint("cnt: %d\n", cnt);
    800008be:	20000593          	li	a1,512
    800008c2:	00007517          	auipc	a0,0x7
    800008c6:	e5650513          	addi	a0,a0,-426 # 80007718 <rfs_i_ops+0x6a0>
    800008ca:	328040ef          	jal	80004bf2 <sprint>
    for (int i = 0; i < cnt; i++) {
    800008ce:	4c81                	li	s9,0
    800008d0:	a879                	j	8000096e <exec_clean_pagetable+0xca>
            pagetable_t page_mid_dir = (pagetable_t)PTE2PA(*pte1);
            for (int j = 0; j < cnt; j++) {
                pte_t* pte2 = page_mid_dir + j;
                if (*pte2 & PTE_V) {
                    pagetable_t page_low_dir = (pagetable_t)PTE2PA(*pte2);
                    for (int k = 0; k < cnt; k++) {
    800008d2:	2485                	addiw	s1,s1,1
    800008d4:	1ff00793          	li	a5,511
    800008d8:	0297ca63          	blt	a5,s1,8000090c <exec_clean_pagetable+0x68>
                        pte_t* pte3 = page_low_dir + k;
    800008dc:	00349413          	slli	s0,s1,0x3
    800008e0:	944e                	add	s0,s0,s3
                        if (*pte3 & PTE_V) {
    800008e2:	6008                	ld	a0,0(s0)
    800008e4:	00157793          	andi	a5,a0,1
    800008e8:	d7ed                	beqz	a5,800008d2 <exec_clean_pagetable+0x2e>
                            uint64 page = PTE2PA(*pte3);
    800008ea:	8129                	srli	a0,a0,0xa
    800008ec:	00c51913          	slli	s2,a0,0xc
                            sprint("inner:%d\n",prot_to_type(PROT_READ | PROT_EXEC, 0));
    800008f0:	85ca                	mv	a1,s2
    800008f2:	00007517          	auipc	a0,0x7
    800008f6:	e3650513          	addi	a0,a0,-458 # 80007728 <rfs_i_ops+0x6b0>
    800008fa:	2f8040ef          	jal	80004bf2 <sprint>
                            free_page((void *)page); // 释放物理页(注意：修改了原先的free_page函数)
    800008fe:	854a                	mv	a0,s2
    80000900:	e2dff0ef          	jal	8000072c <free_page>
                            (*pte3) &= ~PTE_V; // 将页表项置为无效
    80000904:	601c                	ld	a5,0(s0)
    80000906:	9bf9                	andi	a5,a5,-2
    80000908:	e01c                	sd	a5,0(s0)
    8000090a:	b7e1                	j	800008d2 <exec_clean_pagetable+0x2e>
                        }
                    }
                    sprint("midlle:%p\n",page_low_dir);
    8000090c:	85ce                	mv	a1,s3
    8000090e:	00007517          	auipc	a0,0x7
    80000912:	e2a50513          	addi	a0,a0,-470 # 80007738 <rfs_i_ops+0x6c0>
    80000916:	2dc040ef          	jal	80004bf2 <sprint>
                    free_page((void *)page_low_dir);
    8000091a:	854e                	mv	a0,s3
    8000091c:	e11ff0ef          	jal	8000072c <free_page>
                    (*pte2) &= ~PTE_V;
    80000920:	000a3783          	ld	a5,0(s4)
    80000924:	9bf9                	andi	a5,a5,-2
    80000926:	00fa3023          	sd	a5,0(s4)
            for (int j = 0; j < cnt; j++) {
    8000092a:	2a85                	addiw	s5,s5,1
    8000092c:	1ff00793          	li	a5,511
    80000930:	0157cf63          	blt	a5,s5,8000094e <exec_clean_pagetable+0xaa>
                pte_t* pte2 = page_mid_dir + j;
    80000934:	003a9a13          	slli	s4,s5,0x3
    80000938:	9a5a                	add	s4,s4,s6
                if (*pte2 & PTE_V) {
    8000093a:	000a3783          	ld	a5,0(s4)
    8000093e:	0017f713          	andi	a4,a5,1
    80000942:	d765                	beqz	a4,8000092a <exec_clean_pagetable+0x86>
                    pagetable_t page_low_dir = (pagetable_t)PTE2PA(*pte2);
    80000944:	83a9                	srli	a5,a5,0xa
    80000946:	00c79993          	slli	s3,a5,0xc
                    for (int k = 0; k < cnt; k++) {
    8000094a:	4481                	li	s1,0
    8000094c:	b761                	j	800008d4 <exec_clean_pagetable+0x30>
                }
            }
            sprint("outer:%p\n",page_mid_dir);
    8000094e:	85da                	mv	a1,s6
    80000950:	00007517          	auipc	a0,0x7
    80000954:	df850513          	addi	a0,a0,-520 # 80007748 <rfs_i_ops+0x6d0>
    80000958:	29a040ef          	jal	80004bf2 <sprint>
            free_page((void *)page_mid_dir);
    8000095c:	855a                	mv	a0,s6
    8000095e:	dcfff0ef          	jal	8000072c <free_page>
            (*pte1) &= ~PTE_V;
    80000962:	000c3783          	ld	a5,0(s8)
    80000966:	9bf9                	andi	a5,a5,-2
    80000968:	00fc3023          	sd	a5,0(s8)
    for (int i = 0; i < cnt; i++) {
    8000096c:	2c85                	addiw	s9,s9,1
    8000096e:	1ff00793          	li	a5,511
    80000972:	0197cf63          	blt	a5,s9,80000990 <exec_clean_pagetable+0xec>
        pte_t* pte1 = page_dir + i;
    80000976:	003c9c13          	slli	s8,s9,0x3
    8000097a:	9c5e                	add	s8,s8,s7
        if (*pte1 & PTE_V) {
    8000097c:	000c3783          	ld	a5,0(s8)
    80000980:	0017f713          	andi	a4,a5,1
    80000984:	d765                	beqz	a4,8000096c <exec_clean_pagetable+0xc8>
            pagetable_t page_mid_dir = (pagetable_t)PTE2PA(*pte1);
    80000986:	83a9                	srli	a5,a5,0xa
    80000988:	00c79b13          	slli	s6,a5,0xc
            for (int j = 0; j < cnt; j++) {
    8000098c:	4a81                	li	s5,0
    8000098e:	bf79                	j	8000092c <exec_clean_pagetable+0x88>
        }
    }
    sprint("last:%p\n",page_dir);
    80000990:	85de                	mv	a1,s7
    80000992:	00007517          	auipc	a0,0x7
    80000996:	dc650513          	addi	a0,a0,-570 # 80007758 <rfs_i_ops+0x6e0>
    8000099a:	258040ef          	jal	80004bf2 <sprint>
    free_page((void *)page_dir);
    8000099e:	855e                	mv	a0,s7
    800009a0:	d8dff0ef          	jal	8000072c <free_page>
}
    800009a4:	60e6                	ld	ra,88(sp)
    800009a6:	6446                	ld	s0,80(sp)
    800009a8:	64a6                	ld	s1,72(sp)
    800009aa:	6906                	ld	s2,64(sp)
    800009ac:	79e2                	ld	s3,56(sp)
    800009ae:	7a42                	ld	s4,48(sp)
    800009b0:	7aa2                	ld	s5,40(sp)
    800009b2:	7b02                	ld	s6,32(sp)
    800009b4:	6be2                	ld	s7,24(sp)
    800009b6:	6c42                	ld	s8,16(sp)
    800009b8:	6ca2                	ld	s9,8(sp)
    800009ba:	6125                	addi	sp,sp,96
    800009bc:	8082                	ret

00000000800009be <switch_to>:
void switch_to(process* proc) {
    800009be:	1141                	addi	sp,sp,-16
    800009c0:	e406                	sd	ra,8(sp)
  assert(proc);
    800009c2:	c135                	beqz	a0,80000a26 <switch_to+0x68>
    800009c4:	87aa                	mv	a5,a0
  current = proc;
    800009c6:	0000f717          	auipc	a4,0xf
    800009ca:	76a73123          	sd	a0,1890(a4) # 80010128 <current>
  write_csr(stvec, (uint64)smode_trap_vector);
    800009ce:	00005717          	auipc	a4,0x5
    800009d2:	63270713          	addi	a4,a4,1586 # 80006000 <_trap_sec_start>
    800009d6:	10571073          	csrw	stvec,a4
  proc->trapframe->kernel_sp = proc->kstack;      // process's kernel stack
    800009da:	6918                	ld	a4,16(a0)
    800009dc:	6114                	ld	a3,0(a0)
    800009de:	ff74                	sd	a3,248(a4)
  proc->trapframe->kernel_satp = read_csr(satp);  // kernel page table
    800009e0:	180026f3          	csrr	a3,satp
    800009e4:	6918                	ld	a4,16(a0)
    800009e6:	10d73823          	sd	a3,272(a4)
  proc->trapframe->kernel_trap = (uint64)smode_trap_handler;
    800009ea:	6918                	ld	a4,16(a0)
    800009ec:	00002697          	auipc	a3,0x2
    800009f0:	dc868693          	addi	a3,a3,-568 # 800027b4 <smode_trap_handler>
    800009f4:	10d73023          	sd	a3,256(a4)
  unsigned long x = read_csr(sstatus);
    800009f8:	10002773          	csrr	a4,sstatus
  x &= ~SSTATUS_SPP;  // clear SPP to 0 for user mode
    800009fc:	eff77713          	andi	a4,a4,-257
  x |= SSTATUS_SPIE;  // enable interrupts in user mode
    80000a00:	02076713          	ori	a4,a4,32
  write_csr(sstatus, x);
    80000a04:	10071073          	csrw	sstatus,a4
  write_csr(sepc, proc->trapframe->epc);
    80000a08:	6908                	ld	a0,16(a0)
    80000a0a:	10853703          	ld	a4,264(a0)
    80000a0e:	14171073          	csrw	sepc,a4
  uint64 user_satp = MAKE_SATP(proc->pagetable);
    80000a12:	678c                	ld	a1,8(a5)
    80000a14:	81b1                	srli	a1,a1,0xc
  return_to_user(proc->trapframe, user_satp);
    80000a16:	57fd                	li	a5,-1
    80000a18:	17fe                	slli	a5,a5,0x3f
    80000a1a:	8ddd                	or	a1,a1,a5
    80000a1c:	684050ef          	jal	800060a0 <return_to_user>
}
    80000a20:	60a2                	ld	ra,8(sp)
    80000a22:	0141                	addi	sp,sp,16
    80000a24:	8082                	ret
  assert(proc);
    80000a26:	00007697          	auipc	a3,0x7
    80000a2a:	d4268693          	addi	a3,a3,-702 # 80007768 <rfs_i_ops+0x6f0>
    80000a2e:	02700613          	li	a2,39
    80000a32:	00007597          	auipc	a1,0x7
    80000a36:	d3e58593          	addi	a1,a1,-706 # 80007770 <rfs_i_ops+0x6f8>
    80000a3a:	00007517          	auipc	a0,0x7
    80000a3e:	d4e50513          	addi	a0,a0,-690 # 80007788 <rfs_i_ops+0x710>
    80000a42:	1b0040ef          	jal	80004bf2 <sprint>
    80000a46:	6541                	lui	a0,0x10
    80000a48:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    80000a4a:	1c8040ef          	jal	80004c12 <poweroff>

0000000080000a4e <init_proc_pool>:
void init_proc_pool() {
    80000a4e:	1141                	addi	sp,sp,-16
    80000a50:	e406                	sd	ra,8(sp)
  memset( procs, 0, sizeof(process)*NPROC );
    80000a52:	660d                	lui	a2,0x3
    80000a54:	e0060613          	addi	a2,a2,-512 # 2e00 <elf_fpread-0x7fffd200>
    80000a58:	4581                	li	a1,0
    80000a5a:	0000a517          	auipc	a0,0xa
    80000a5e:	9b650513          	addi	a0,a0,-1610 # 8000a410 <procs>
    80000a62:	6f2030ef          	jal	80004154 <memset>
  for (int i = 0; i < NPROC; ++i) {
    80000a66:	4781                	li	a5,0
    80000a68:	a839                	j	80000a86 <init_proc_pool+0x38>
    procs[i].status = FREE;
    80000a6a:	17000693          	li	a3,368
    80000a6e:	02d786b3          	mul	a3,a5,a3
    80000a72:	0000a717          	auipc	a4,0xa
    80000a76:	99e70713          	addi	a4,a4,-1634 # 8000a410 <procs>
    80000a7a:	9736                	add	a4,a4,a3
    80000a7c:	14072423          	sw	zero,328(a4)
    procs[i].pid = i;
    80000a80:	14f73023          	sd	a5,320(a4)
  for (int i = 0; i < NPROC; ++i) {
    80000a84:	2785                	addiw	a5,a5,1 # 8000001 <elf_fpread-0x77ffffff>
    80000a86:	477d                	li	a4,31
    80000a88:	fef751e3          	bge	a4,a5,80000a6a <init_proc_pool+0x1c>
}
    80000a8c:	60a2                	ld	ra,8(sp)
    80000a8e:	0141                	addi	sp,sp,16
    80000a90:	8082                	ret

0000000080000a92 <alloc_process>:
process* alloc_process() {
    80000a92:	7179                	addi	sp,sp,-48
    80000a94:	f406                	sd	ra,40(sp)
    80000a96:	f022                	sd	s0,32(sp)
    80000a98:	ec26                	sd	s1,24(sp)
    80000a9a:	e84a                	sd	s2,16(sp)
    80000a9c:	e44e                	sd	s3,8(sp)
  for( i=0; i<NPROC; i++ )
    80000a9e:	4401                	li	s0,0
    80000aa0:	47fd                	li	a5,31
    80000aa2:	0287c063          	blt	a5,s0,80000ac2 <alloc_process+0x30>
    if( procs[i].status == FREE ) break;
    80000aa6:	17000713          	li	a4,368
    80000aaa:	02e40733          	mul	a4,s0,a4
    80000aae:	0000a797          	auipc	a5,0xa
    80000ab2:	96278793          	addi	a5,a5,-1694 # 8000a410 <procs>
    80000ab6:	97ba                	add	a5,a5,a4
    80000ab8:	1487a783          	lw	a5,328(a5)
    80000abc:	c399                	beqz	a5,80000ac2 <alloc_process+0x30>
  for( i=0; i<NPROC; i++ )
    80000abe:	2405                	addiw	s0,s0,1
    80000ac0:	b7c5                	j	80000aa0 <alloc_process+0xe>
  if( i>=NPROC ){
    80000ac2:	47fd                	li	a5,31
    80000ac4:	1487c263          	blt	a5,s0,80000c08 <alloc_process+0x176>
  procs[i].trapframe = (trapframe *)alloc_page();  //trapframe, used to save context
    80000ac8:	d15ff0ef          	jal	800007dc <alloc_page>
    80000acc:	17000793          	li	a5,368
    80000ad0:	02f40433          	mul	s0,s0,a5
    80000ad4:	0000a797          	auipc	a5,0xa
    80000ad8:	93c78793          	addi	a5,a5,-1732 # 8000a410 <procs>
    80000adc:	943e                	add	s0,s0,a5
    80000ade:	e808                	sd	a0,16(s0)
  memset(procs[i].trapframe, 0, sizeof(trapframe));
    80000ae0:	11800613          	li	a2,280
    80000ae4:	4581                	li	a1,0
    80000ae6:	66e030ef          	jal	80004154 <memset>
  procs[i].pagetable = (pagetable_t)alloc_page();
    80000aea:	cf3ff0ef          	jal	800007dc <alloc_page>
    80000aee:	e408                	sd	a0,8(s0)
  memset((void *)procs[i].pagetable, 0, PGSIZE);
    80000af0:	6605                	lui	a2,0x1
    80000af2:	4581                	li	a1,0
    80000af4:	660030ef          	jal	80004154 <memset>
  procs[i].kstack = (uint64)alloc_page() + PGSIZE;   //user kernel stack top
    80000af8:	ce5ff0ef          	jal	800007dc <alloc_page>
    80000afc:	6785                	lui	a5,0x1
    80000afe:	953e                	add	a0,a0,a5
    80000b00:	e008                	sd	a0,0(s0)
  uint64 user_stack = (uint64)alloc_page();       //phisical address of user stack bottom
    80000b02:	cdbff0ef          	jal	800007dc <alloc_page>
    80000b06:	84aa                	mv	s1,a0
  procs[i].trapframe->regs.sp = USER_STACK_TOP;  //virtual address of user stack top
    80000b08:	681c                	ld	a5,16(s0)
    80000b0a:	7ffff737          	lui	a4,0x7ffff
    80000b0e:	e798                	sd	a4,8(a5)
  procs[i].mapped_info = (mapped_region*)alloc_page();
    80000b10:	ccdff0ef          	jal	800007dc <alloc_page>
    80000b14:	ec08                	sd	a0,24(s0)
  memset( procs[i].mapped_info, 0, PGSIZE );
    80000b16:	6605                	lui	a2,0x1
    80000b18:	4581                	li	a1,0
    80000b1a:	63a030ef          	jal	80004154 <memset>
  user_vm_map((pagetable_t)procs[i].pagetable, USER_STACK_TOP - PGSIZE, PGSIZE,
    80000b1e:	00843903          	ld	s2,8(s0)
    user_stack, prot_to_type(PROT_WRITE | PROT_READ, 1));
    80000b22:	4585                	li	a1,1
    80000b24:	450d                	li	a0,3
    80000b26:	50f020ef          	jal	80003834 <prot_to_type>
  user_vm_map((pagetable_t)procs[i].pagetable, USER_STACK_TOP - PGSIZE, PGSIZE,
    80000b2a:	0005071b          	sext.w	a4,a0
    80000b2e:	86a6                	mv	a3,s1
    80000b30:	6605                	lui	a2,0x1
    80000b32:	7fffe5b7          	lui	a1,0x7fffe
    80000b36:	854a                	mv	a0,s2
    80000b38:	74b020ef          	jal	80003a82 <user_vm_map>
  procs[i].mapped_info[STACK_SEGMENT].va = USER_STACK_TOP - PGSIZE;
    80000b3c:	6c1c                	ld	a5,24(s0)
    80000b3e:	7fffe737          	lui	a4,0x7fffe
    80000b42:	e398                	sd	a4,0(a5)
  procs[i].mapped_info[STACK_SEGMENT].npages = 1;
    80000b44:	6c1c                	ld	a5,24(s0)
    80000b46:	4485                	li	s1,1
    80000b48:	c784                	sw	s1,8(a5)
  procs[i].mapped_info[STACK_SEGMENT].seg_type = STACK_SEGMENT;
    80000b4a:	6c1c                	ld	a5,24(s0)
    80000b4c:	0007a623          	sw	zero,12(a5) # 100c <elf_fpread-0x7fffeff4>
  user_vm_map((pagetable_t)procs[i].pagetable, (uint64)procs[i].trapframe, PGSIZE,
    80000b50:	00843983          	ld	s3,8(s0)
    80000b54:	01043903          	ld	s2,16(s0)
    (uint64)procs[i].trapframe, prot_to_type(PROT_WRITE | PROT_READ, 0));
    80000b58:	4581                	li	a1,0
    80000b5a:	450d                	li	a0,3
    80000b5c:	4d9020ef          	jal	80003834 <prot_to_type>
  user_vm_map((pagetable_t)procs[i].pagetable, (uint64)procs[i].trapframe, PGSIZE,
    80000b60:	0005071b          	sext.w	a4,a0
    80000b64:	86ca                	mv	a3,s2
    80000b66:	6605                	lui	a2,0x1
    80000b68:	85ca                	mv	a1,s2
    80000b6a:	854e                	mv	a0,s3
    80000b6c:	717020ef          	jal	80003a82 <user_vm_map>
  procs[i].mapped_info[CONTEXT_SEGMENT].va = (uint64)procs[i].trapframe;
    80000b70:	6818                	ld	a4,16(s0)
    80000b72:	6c1c                	ld	a5,24(s0)
    80000b74:	eb98                	sd	a4,16(a5)
  procs[i].mapped_info[CONTEXT_SEGMENT].npages = 1;
    80000b76:	6c1c                	ld	a5,24(s0)
    80000b78:	cf84                	sw	s1,24(a5)
  procs[i].mapped_info[CONTEXT_SEGMENT].seg_type = CONTEXT_SEGMENT;
    80000b7a:	6c1c                	ld	a5,24(s0)
    80000b7c:	cfc4                	sw	s1,28(a5)
  user_vm_map((pagetable_t)procs[i].pagetable, (uint64)trap_sec_start, PGSIZE,
    80000b7e:	00843983          	ld	s3,8(s0)
    80000b82:	00005917          	auipc	s2,0x5
    80000b86:	47e90913          	addi	s2,s2,1150 # 80006000 <_trap_sec_start>
    (uint64)trap_sec_start, prot_to_type(PROT_READ | PROT_EXEC, 0));
    80000b8a:	4581                	li	a1,0
    80000b8c:	4515                	li	a0,5
    80000b8e:	4a7020ef          	jal	80003834 <prot_to_type>
  user_vm_map((pagetable_t)procs[i].pagetable, (uint64)trap_sec_start, PGSIZE,
    80000b92:	0005071b          	sext.w	a4,a0
    80000b96:	86ca                	mv	a3,s2
    80000b98:	6605                	lui	a2,0x1
    80000b9a:	85ca                	mv	a1,s2
    80000b9c:	854e                	mv	a0,s3
    80000b9e:	6e5020ef          	jal	80003a82 <user_vm_map>
  procs[i].mapped_info[SYSTEM_SEGMENT].va = (uint64)trap_sec_start;
    80000ba2:	6c1c                	ld	a5,24(s0)
    80000ba4:	0327b023          	sd	s2,32(a5)
  procs[i].mapped_info[SYSTEM_SEGMENT].npages = 1;
    80000ba8:	6c1c                	ld	a5,24(s0)
    80000baa:	d784                	sw	s1,40(a5)
  procs[i].mapped_info[SYSTEM_SEGMENT].seg_type = SYSTEM_SEGMENT;
    80000bac:	6c1c                	ld	a5,24(s0)
    80000bae:	4709                	li	a4,2
    80000bb0:	d7d8                	sw	a4,44(a5)
  sprint("in alloc_proc. user frame 0x%lx, user stack 0x%lx, user kstack 0x%lx \n",
    80000bb2:	680c                	ld	a1,16(s0)
    80000bb4:	6014                	ld	a3,0(s0)
    80000bb6:	6590                	ld	a2,8(a1)
    80000bb8:	00007517          	auipc	a0,0x7
    80000bbc:	c2050513          	addi	a0,a0,-992 # 800077d8 <rfs_i_ops+0x760>
    80000bc0:	032040ef          	jal	80004bf2 <sprint>
  procs[i].user_heap.heap_top = USER_FREE_ADDRESS_START;
    80000bc4:	004007b7          	lui	a5,0x400
    80000bc8:	f41c                	sd	a5,40(s0)
  procs[i].user_heap.heap_bottom = USER_FREE_ADDRESS_START;
    80000bca:	f81c                	sd	a5,48(s0)
  procs[i].user_heap.free_pages_count = 0;
    80000bcc:	12042c23          	sw	zero,312(s0)
  procs[i].mapped_info[HEAP_SEGMENT].va = USER_FREE_ADDRESS_START;
    80000bd0:	6c18                	ld	a4,24(s0)
    80000bd2:	fb1c                	sd	a5,48(a4)
  procs[i].mapped_info[HEAP_SEGMENT].npages = 0;  // no pages are mapped to heap yet.
    80000bd4:	6c1c                	ld	a5,24(s0)
    80000bd6:	0207ac23          	sw	zero,56(a5) # 400038 <elf_fpread-0x7fbfffc8>
  procs[i].mapped_info[HEAP_SEGMENT].seg_type = HEAP_SEGMENT;
    80000bda:	6c1c                	ld	a5,24(s0)
    80000bdc:	470d                	li	a4,3
    80000bde:	dfd8                	sw	a4,60(a5)
  procs[i].total_mapped_region = 4;
    80000be0:	4791                	li	a5,4
    80000be2:	d01c                	sw	a5,32(s0)
  procs[i].pfiles = init_proc_file_management();
    80000be4:	3d4000ef          	jal	80000fb8 <init_proc_file_management>
    80000be8:	16a43423          	sd	a0,360(s0)
  sprint("in alloc_proc. build proc_file_management successfully.\n");
    80000bec:	00007517          	auipc	a0,0x7
    80000bf0:	c3450513          	addi	a0,a0,-972 # 80007820 <rfs_i_ops+0x7a8>
    80000bf4:	7ff030ef          	jal	80004bf2 <sprint>
}
    80000bf8:	8522                	mv	a0,s0
    80000bfa:	70a2                	ld	ra,40(sp)
    80000bfc:	7402                	ld	s0,32(sp)
    80000bfe:	64e2                	ld	s1,24(sp)
    80000c00:	6942                	ld	s2,16(sp)
    80000c02:	69a2                	ld	s3,8(sp)
    80000c04:	6145                	addi	sp,sp,48
    80000c06:	8082                	ret
    panic( "cannot find any free process structure.\n" );
    80000c08:	00007517          	auipc	a0,0x7
    80000c0c:	ba050513          	addi	a0,a0,-1120 # 800077a8 <rfs_i_ops+0x730>
    80000c10:	086040ef          	jal	80004c96 <do_panic>

0000000080000c14 <free_process>:
  proc->status = ZOMBIE;
    80000c14:	4791                	li	a5,4
    80000c16:	14f52423          	sw	a5,328(a0)
}
    80000c1a:	4501                	li	a0,0
    80000c1c:	8082                	ret

0000000080000c1e <do_fork>:
{
    80000c1e:	7155                	addi	sp,sp,-208
    80000c20:	e586                	sd	ra,200(sp)
    80000c22:	e1a2                	sd	s0,192(sp)
    80000c24:	fd26                	sd	s1,184(sp)
    80000c26:	f94a                	sd	s2,176(sp)
    80000c28:	f54e                	sd	s3,168(sp)
    80000c2a:	f152                	sd	s4,160(sp)
    80000c2c:	ed56                	sd	s5,152(sp)
    80000c2e:	e95a                	sd	s6,144(sp)
    80000c30:	e55e                	sd	s7,136(sp)
    80000c32:	842a                	mv	s0,a0
  sprint( "will fork a child from parent %d.\n", parent->pid );
    80000c34:	14053583          	ld	a1,320(a0)
    80000c38:	00007517          	auipc	a0,0x7
    80000c3c:	c2850513          	addi	a0,a0,-984 # 80007860 <rfs_i_ops+0x7e8>
    80000c40:	7b3030ef          	jal	80004bf2 <sprint>
  process* child = alloc_process();
    80000c44:	e4fff0ef          	jal	80000a92 <alloc_process>
    80000c48:	84aa                	mv	s1,a0
  for( int i=0; i<parent->total_mapped_region; i++ ){
    80000c4a:	4901                	li	s2,0
    80000c4c:	a05d                	j	80000cf2 <do_fork+0xd4>
    switch( parent->mapped_info[i].seg_type ){
    80000c4e:	4691                	li	a3,4
    80000c50:	0ad71063          	bne	a4,a3,80000cf0 <do_fork+0xd2>
		  user_vm_map((pagetable_t) child->pagetable, parent->mapped_info[i].va,
    80000c54:	0084bb83          	ld	s7,8(s1)
    80000c58:	0007bb03          	ld	s6,0(a5)
			  parent->mapped_info[i].npages * PGSIZE,
    80000c5c:	0087aa03          	lw	s4,8(a5)
    80000c60:	00ca1a1b          	slliw	s4,s4,0xc
		  user_vm_map((pagetable_t) child->pagetable, parent->mapped_info[i].va,
    80000c64:	85da                	mv	a1,s6
    80000c66:	6408                	ld	a0,8(s0)
    80000c68:	50d020ef          	jal	80003974 <lookup_pa>
    80000c6c:	8aaa                	mv	s5,a0
			  prot_to_type(PROT_READ | PROT_EXEC, 1));
    80000c6e:	4585                	li	a1,1
    80000c70:	4515                	li	a0,5
    80000c72:	3c3020ef          	jal	80003834 <prot_to_type>
		  user_vm_map((pagetable_t) child->pagetable, parent->mapped_info[i].va,
    80000c76:	0005071b          	sext.w	a4,a0
    80000c7a:	86d6                	mv	a3,s5
    80000c7c:	020a1613          	slli	a2,s4,0x20
    80000c80:	9201                	srli	a2,a2,0x20
    80000c82:	85da                	mv	a1,s6
    80000c84:	855e                	mv	a0,s7
    80000c86:	5fd020ef          	jal	80003a82 <user_vm_map>
        child->mapped_info[child->total_mapped_region].va = parent->mapped_info[i].va;
    80000c8a:	6c18                	ld	a4,24(s0)
    80000c8c:	974e                	add	a4,a4,s3
    80000c8e:	6c9c                	ld	a5,24(s1)
    80000c90:	5094                	lw	a3,32(s1)
    80000c92:	0692                	slli	a3,a3,0x4
    80000c94:	97b6                	add	a5,a5,a3
    80000c96:	6318                	ld	a4,0(a4)
    80000c98:	e398                	sd	a4,0(a5)
          parent->mapped_info[i].npages;
    80000c9a:	6c18                	ld	a4,24(s0)
    80000c9c:	974e                	add	a4,a4,s3
        child->mapped_info[child->total_mapped_region].npages =
    80000c9e:	6c9c                	ld	a5,24(s1)
    80000ca0:	5094                	lw	a3,32(s1)
    80000ca2:	0692                	slli	a3,a3,0x4
    80000ca4:	97b6                	add	a5,a5,a3
          parent->mapped_info[i].npages;
    80000ca6:	4718                	lw	a4,8(a4)
        child->mapped_info[child->total_mapped_region].npages =
    80000ca8:	c798                	sw	a4,8(a5)
        child->mapped_info[child->total_mapped_region].seg_type = CODE_SEGMENT;
    80000caa:	6c9c                	ld	a5,24(s1)
    80000cac:	5098                	lw	a4,32(s1)
    80000cae:	0712                	slli	a4,a4,0x4
    80000cb0:	97ba                	add	a5,a5,a4
    80000cb2:	4711                	li	a4,4
    80000cb4:	c7d8                	sw	a4,12(a5)
        child->total_mapped_region++;
    80000cb6:	509c                	lw	a5,32(s1)
    80000cb8:	2785                	addiw	a5,a5,1
    80000cba:	d09c                	sw	a5,32(s1)
        break;
    80000cbc:	a815                	j	80000cf0 <do_fork+0xd2>
        *child->trapframe = *parent->trapframe;
    80000cbe:	6814                	ld	a3,16(s0)
    80000cc0:	87b6                	mv	a5,a3
    80000cc2:	6898                	ld	a4,16(s1)
    80000cc4:	11868693          	addi	a3,a3,280
    80000cc8:	0007b883          	ld	a7,0(a5)
    80000ccc:	0087b803          	ld	a6,8(a5)
    80000cd0:	6b88                	ld	a0,16(a5)
    80000cd2:	6f8c                	ld	a1,24(a5)
    80000cd4:	7390                	ld	a2,32(a5)
    80000cd6:	01173023          	sd	a7,0(a4) # 7fffe000 <elf_fpread-0x2000>
    80000cda:	01073423          	sd	a6,8(a4)
    80000cde:	eb08                	sd	a0,16(a4)
    80000ce0:	ef0c                	sd	a1,24(a4)
    80000ce2:	f310                	sd	a2,32(a4)
    80000ce4:	02878793          	addi	a5,a5,40
    80000ce8:	02870713          	addi	a4,a4,40
    80000cec:	fcd79ee3          	bne	a5,a3,80000cc8 <do_fork+0xaa>
  for( int i=0; i<parent->total_mapped_region; i++ ){
    80000cf0:	2905                	addiw	s2,s2,1
    80000cf2:	501c                	lw	a5,32(s0)
    80000cf4:	10f95663          	bge	s2,a5,80000e00 <do_fork+0x1e2>
    switch( parent->mapped_info[i].seg_type ){
    80000cf8:	6c1c                	ld	a5,24(s0)
    80000cfa:	00491993          	slli	s3,s2,0x4
    80000cfe:	97ce                	add	a5,a5,s3
    80000d00:	47d8                	lw	a4,12(a5)
    80000d02:	468d                	li	a3,3
    80000d04:	02d70b63          	beq	a4,a3,80000d3a <do_fork+0x11c>
    80000d08:	f4e6e3e3          	bltu	a3,a4,80000c4e <do_fork+0x30>
    80000d0c:	c709                	beqz	a4,80000d16 <do_fork+0xf8>
    80000d0e:	4785                	li	a5,1
    80000d10:	faf707e3          	beq	a4,a5,80000cbe <do_fork+0xa0>
    80000d14:	bff1                	j	80000cf0 <do_fork+0xd2>
        memcpy( (void*)lookup_pa(child->pagetable, child->mapped_info[STACK_SEGMENT].va),
    80000d16:	6c9c                	ld	a5,24(s1)
    80000d18:	638c                	ld	a1,0(a5)
    80000d1a:	6488                	ld	a0,8(s1)
    80000d1c:	459020ef          	jal	80003974 <lookup_pa>
    80000d20:	8a2a                	mv	s4,a0
          (void*)lookup_pa(parent->pagetable, parent->mapped_info[i].va), PGSIZE );
    80000d22:	6c1c                	ld	a5,24(s0)
    80000d24:	97ce                	add	a5,a5,s3
    80000d26:	638c                	ld	a1,0(a5)
    80000d28:	6408                	ld	a0,8(s0)
    80000d2a:	44b020ef          	jal	80003974 <lookup_pa>
    80000d2e:	85aa                	mv	a1,a0
        memcpy( (void*)lookup_pa(child->pagetable, child->mapped_info[STACK_SEGMENT].va),
    80000d30:	6605                	lui	a2,0x1
    80000d32:	8552                	mv	a0,s4
    80000d34:	3e8030ef          	jal	8000411c <memcpy>
        break;
    80000d38:	bf65                	j	80000cf0 <do_fork+0xd2>
        memset(free_block_filter, 0, MAX_HEAP_PAGES);
    80000d3a:	02000613          	li	a2,32
    80000d3e:	4581                	li	a1,0
    80000d40:	850a                	mv	a0,sp
    80000d42:	412030ef          	jal	80004154 <memset>
        uint64 heap_bottom = parent->user_heap.heap_bottom;
    80000d46:	03043983          	ld	s3,48(s0)
        for (int i = 0; i < parent->user_heap.free_pages_count; i++) {
    80000d4a:	4701                	li	a4,0
    80000d4c:	a015                	j	80000d70 <do_fork+0x152>
          int index = (parent->user_heap.free_pages_address[i] - heap_bottom) / PGSIZE;
    80000d4e:	00670793          	addi	a5,a4,6
    80000d52:	078e                	slli	a5,a5,0x3
    80000d54:	97a2                	add	a5,a5,s0
    80000d56:	679c                	ld	a5,8(a5)
    80000d58:	413787b3          	sub	a5,a5,s3
    80000d5c:	83b1                	srli	a5,a5,0xc
    80000d5e:	2781                	sext.w	a5,a5
          free_block_filter[index] = 1;
    80000d60:	078a                	slli	a5,a5,0x2
    80000d62:	08078793          	addi	a5,a5,128
    80000d66:	978a                	add	a5,a5,sp
    80000d68:	4685                	li	a3,1
    80000d6a:	f8d7a023          	sw	a3,-128(a5)
        for (int i = 0; i < parent->user_heap.free_pages_count; i++) {
    80000d6e:	2705                	addiw	a4,a4,1
    80000d70:	13842683          	lw	a3,312(s0)
    80000d74:	0007079b          	sext.w	a5,a4
    80000d78:	fcd7ebe3          	bltu	a5,a3,80000d4e <do_fork+0x130>
        for (uint64 heap_block = current->user_heap.heap_bottom;
    80000d7c:	0000f797          	auipc	a5,0xf
    80000d80:	3ac7b783          	ld	a5,940(a5) # 80010128 <current>
    80000d84:	0307ba03          	ld	s4,48(a5)
    80000d88:	a019                	j	80000d8e <do_fork+0x170>
             heap_block < current->user_heap.heap_top; heap_block += PGSIZE) {
    80000d8a:	6785                	lui	a5,0x1
    80000d8c:	9a3e                	add	s4,s4,a5
    80000d8e:	0000f797          	auipc	a5,0xf
    80000d92:	39a7b783          	ld	a5,922(a5) # 80010128 <current>
    80000d96:	779c                	ld	a5,40(a5)
    80000d98:	04fa7763          	bgeu	s4,a5,80000de6 <do_fork+0x1c8>
          if (free_block_filter[(heap_block - heap_bottom) / PGSIZE])  // skip free blocks
    80000d9c:	413a07b3          	sub	a5,s4,s3
    80000da0:	83b1                	srli	a5,a5,0xc
    80000da2:	078a                	slli	a5,a5,0x2
    80000da4:	08078793          	addi	a5,a5,128
    80000da8:	978a                	add	a5,a5,sp
    80000daa:	f807a783          	lw	a5,-128(a5)
    80000dae:	fff1                	bnez	a5,80000d8a <do_fork+0x16c>
          void* child_pa = alloc_page();
    80000db0:	a2dff0ef          	jal	800007dc <alloc_page>
    80000db4:	8aaa                	mv	s5,a0
          memcpy(child_pa, (void*)lookup_pa(parent->pagetable, heap_block), PGSIZE);
    80000db6:	85d2                	mv	a1,s4
    80000db8:	6408                	ld	a0,8(s0)
    80000dba:	3bb020ef          	jal	80003974 <lookup_pa>
    80000dbe:	85aa                	mv	a1,a0
    80000dc0:	6605                	lui	a2,0x1
    80000dc2:	8556                	mv	a0,s5
    80000dc4:	358030ef          	jal	8000411c <memcpy>
          user_vm_map((pagetable_t)child->pagetable, heap_block, PGSIZE, (uint64)child_pa,
    80000dc8:	0084bb03          	ld	s6,8(s1)
                      prot_to_type(PROT_WRITE | PROT_READ, 1));
    80000dcc:	4585                	li	a1,1
    80000dce:	450d                	li	a0,3
    80000dd0:	265020ef          	jal	80003834 <prot_to_type>
          user_vm_map((pagetable_t)child->pagetable, heap_block, PGSIZE, (uint64)child_pa,
    80000dd4:	0005071b          	sext.w	a4,a0
    80000dd8:	86d6                	mv	a3,s5
    80000dda:	6605                	lui	a2,0x1
    80000ddc:	85d2                	mv	a1,s4
    80000dde:	855a                	mv	a0,s6
    80000de0:	4a3020ef          	jal	80003a82 <user_vm_map>
    80000de4:	b75d                	j	80000d8a <do_fork+0x16c>
        child->mapped_info[HEAP_SEGMENT].npages = parent->mapped_info[HEAP_SEGMENT].npages;
    80000de6:	6c18                	ld	a4,24(s0)
    80000de8:	6c9c                	ld	a5,24(s1)
    80000dea:	5f18                	lw	a4,56(a4)
    80000dec:	df98                	sw	a4,56(a5)
        memcpy((void*)&child->user_heap, (void*)&parent->user_heap, sizeof(parent->user_heap));
    80000dee:	11800613          	li	a2,280
    80000df2:	02840593          	addi	a1,s0,40
    80000df6:	02848513          	addi	a0,s1,40
    80000dfa:	322030ef          	jal	8000411c <memcpy>
        break;
    80000dfe:	bdcd                	j	80000cf0 <do_fork+0xd2>
  child->status = READY;
    80000e00:	4785                	li	a5,1
    80000e02:	14f4a423          	sw	a5,328(s1)
  child->trapframe->regs.a0 = 0;
    80000e06:	689c                	ld	a5,16(s1)
    80000e08:	0407b423          	sd	zero,72(a5)
  child->parent = parent;
    80000e0c:	1484b823          	sd	s0,336(s1)
  insert_to_ready_queue( child );
    80000e10:	8526                	mv	a0,s1
    80000e12:	742010ef          	jal	80002554 <insert_to_ready_queue>
}
    80000e16:	1404a503          	lw	a0,320(s1)
    80000e1a:	60ae                	ld	ra,200(sp)
    80000e1c:	640e                	ld	s0,192(sp)
    80000e1e:	74ea                	ld	s1,184(sp)
    80000e20:	794a                	ld	s2,176(sp)
    80000e22:	79aa                	ld	s3,168(sp)
    80000e24:	7a0a                	ld	s4,160(sp)
    80000e26:	6aea                	ld	s5,152(sp)
    80000e28:	6b4a                	ld	s6,144(sp)
    80000e2a:	6baa                	ld	s7,136(sp)
    80000e2c:	6169                	addi	sp,sp,208
    80000e2e:	8082                	ret

0000000080000e30 <exec_clean>:

void exec_clean(process* p) {
    80000e30:	7179                	addi	sp,sp,-48
    80000e32:	f406                	sd	ra,40(sp)
    80000e34:	f022                	sd	s0,32(sp)
    80000e36:	ec26                	sd	s1,24(sp)
    80000e38:	e84a                	sd	s2,16(sp)
    80000e3a:	e44e                	sd	s3,8(sp)
    80000e3c:	842a                	mv	s0,a0
    // 释放原先内存
    exec_clean_pagetable(p->pagetable);
    80000e3e:	6508                	ld	a0,8(a0)
    80000e40:	a65ff0ef          	jal	800008a4 <exec_clean_pagetable>
    
    // init proc[i]'s vm space
    p->trapframe = (trapframe *)alloc_page(); // trapframe, used to save context 
    80000e44:	999ff0ef          	jal	800007dc <alloc_page>
    80000e48:	e808                	sd	a0,16(s0)
    memset(p->trapframe, 0, sizeof(trapframe));
    80000e4a:	11800613          	li	a2,280
    80000e4e:	4581                	li	a1,0
    80000e50:	304030ef          	jal	80004154 <memset>

    // page directory
    p->pagetable = (pagetable_t)alloc_page();
    80000e54:	989ff0ef          	jal	800007dc <alloc_page>
    80000e58:	e408                	sd	a0,8(s0)
    memset((void *)p->pagetable, 0, PGSIZE);
    80000e5a:	6605                	lui	a2,0x1
    80000e5c:	4581                	li	a1,0
    80000e5e:	2f6030ef          	jal	80004154 <memset>

    p->kstack = (uint64)alloc_page() + PGSIZE; // user kernel stack top
    80000e62:	97bff0ef          	jal	800007dc <alloc_page>
    80000e66:	6785                	lui	a5,0x1
    80000e68:	953e                	add	a0,a0,a5
    80000e6a:	e008                	sd	a0,0(s0)
    uint64 user_stack = (uint64)alloc_page();        // phisical address of user stack bottom
    80000e6c:	971ff0ef          	jal	800007dc <alloc_page>
    80000e70:	84aa                	mv	s1,a0
    p->trapframe->regs.sp = USER_STACK_TOP;    // virtual address of user stack top
    80000e72:	681c                	ld	a5,16(s0)
    80000e74:	7ffff737          	lui	a4,0x7ffff
    80000e78:	e798                	sd	a4,8(a5)

    // allocates a page to record memory regions (segments)
    p->mapped_info = (mapped_region *)alloc_page();
    80000e7a:	963ff0ef          	jal	800007dc <alloc_page>
    80000e7e:	ec08                	sd	a0,24(s0)
    memset(p->mapped_info, 0, PGSIZE);
    80000e80:	6605                	lui	a2,0x1
    80000e82:	4581                	li	a1,0
    80000e84:	2d0030ef          	jal	80004154 <memset>

    // map user stack in userspace
    user_vm_map((pagetable_t)p->pagetable, USER_STACK_TOP - PGSIZE, PGSIZE,
    80000e88:	00843903          	ld	s2,8(s0)
                user_stack, prot_to_type(PROT_WRITE | PROT_READ, 1));
    80000e8c:	4585                	li	a1,1
    80000e8e:	450d                	li	a0,3
    80000e90:	1a5020ef          	jal	80003834 <prot_to_type>
    user_vm_map((pagetable_t)p->pagetable, USER_STACK_TOP - PGSIZE, PGSIZE,
    80000e94:	0005071b          	sext.w	a4,a0
    80000e98:	86a6                	mv	a3,s1
    80000e9a:	6605                	lui	a2,0x1
    80000e9c:	7fffe5b7          	lui	a1,0x7fffe
    80000ea0:	854a                	mv	a0,s2
    80000ea2:	3e1020ef          	jal	80003a82 <user_vm_map>
    p->mapped_info[STACK_SEGMENT].va = USER_STACK_TOP - PGSIZE;
    80000ea6:	6c1c                	ld	a5,24(s0)
    80000ea8:	7fffe737          	lui	a4,0x7fffe
    80000eac:	e398                	sd	a4,0(a5)
    p->mapped_info[STACK_SEGMENT].npages = 1;
    80000eae:	6c1c                	ld	a5,24(s0)
    80000eb0:	4485                	li	s1,1
    80000eb2:	c784                	sw	s1,8(a5)
    p->mapped_info[STACK_SEGMENT].seg_type = STACK_SEGMENT;
    80000eb4:	6c1c                	ld	a5,24(s0)
    80000eb6:	0007a623          	sw	zero,12(a5) # 100c <elf_fpread-0x7fffeff4>

    // map trapframe in user space (direct mapping as in kernel space).
    user_vm_map((pagetable_t)p->pagetable, (uint64)p->trapframe, PGSIZE, // trapframe的物理地址等于虚拟地址
    80000eba:	00843983          	ld	s3,8(s0)
    80000ebe:	01043903          	ld	s2,16(s0)
                (uint64)p->trapframe, prot_to_type(PROT_WRITE | PROT_READ, 0));
    80000ec2:	4581                	li	a1,0
    80000ec4:	450d                	li	a0,3
    80000ec6:	16f020ef          	jal	80003834 <prot_to_type>
    user_vm_map((pagetable_t)p->pagetable, (uint64)p->trapframe, PGSIZE, // trapframe的物理地址等于虚拟地址
    80000eca:	0005071b          	sext.w	a4,a0
    80000ece:	86ca                	mv	a3,s2
    80000ed0:	6605                	lui	a2,0x1
    80000ed2:	85ca                	mv	a1,s2
    80000ed4:	854e                	mv	a0,s3
    80000ed6:	3ad020ef          	jal	80003a82 <user_vm_map>
    p->mapped_info[CONTEXT_SEGMENT].va = (uint64)p->trapframe;
    80000eda:	6818                	ld	a4,16(s0)
    80000edc:	6c1c                	ld	a5,24(s0)
    80000ede:	eb98                	sd	a4,16(a5)
    p->mapped_info[CONTEXT_SEGMENT].npages = 1;
    80000ee0:	6c1c                	ld	a5,24(s0)
    80000ee2:	cf84                	sw	s1,24(a5)
    p->mapped_info[CONTEXT_SEGMENT].seg_type = CONTEXT_SEGMENT;
    80000ee4:	6c1c                	ld	a5,24(s0)
    80000ee6:	cfc4                	sw	s1,28(a5)

    // map S-mode trap vector section in user space (direct mapping as in kernel space)
    // we assume that the size of usertrap.S is smaller than a page.
    user_vm_map((pagetable_t)p->pagetable, (uint64)trap_sec_start, PGSIZE,
    80000ee8:	00843983          	ld	s3,8(s0)
    80000eec:	00005917          	auipc	s2,0x5
    80000ef0:	11490913          	addi	s2,s2,276 # 80006000 <_trap_sec_start>
                (uint64)trap_sec_start, prot_to_type(PROT_READ | PROT_EXEC, 0));
    80000ef4:	4581                	li	a1,0
    80000ef6:	4515                	li	a0,5
    80000ef8:	13d020ef          	jal	80003834 <prot_to_type>
    user_vm_map((pagetable_t)p->pagetable, (uint64)trap_sec_start, PGSIZE,
    80000efc:	0005071b          	sext.w	a4,a0
    80000f00:	86ca                	mv	a3,s2
    80000f02:	6605                	lui	a2,0x1
    80000f04:	85ca                	mv	a1,s2
    80000f06:	854e                	mv	a0,s3
    80000f08:	37b020ef          	jal	80003a82 <user_vm_map>
    p->mapped_info[SYSTEM_SEGMENT].va = (uint64)trap_sec_start;
    80000f0c:	6c1c                	ld	a5,24(s0)
    80000f0e:	0327b023          	sd	s2,32(a5)
    p->mapped_info[SYSTEM_SEGMENT].npages = 1;
    80000f12:	6c1c                	ld	a5,24(s0)
    80000f14:	d784                	sw	s1,40(a5)
    p->mapped_info[SYSTEM_SEGMENT].seg_type = SYSTEM_SEGMENT;
    80000f16:	6c1c                	ld	a5,24(s0)
    80000f18:	4709                	li	a4,2
    80000f1a:	d7d8                	sw	a4,44(a5)

    // initialize the process's heap manager
    p->user_heap.heap_top = USER_FREE_ADDRESS_START;
    80000f1c:	004007b7          	lui	a5,0x400
    80000f20:	f41c                	sd	a5,40(s0)
    p->user_heap.heap_bottom = USER_FREE_ADDRESS_START;
    80000f22:	f81c                	sd	a5,48(s0)
    p->user_heap.free_pages_count = 0;
    80000f24:	12042c23          	sw	zero,312(s0)

    // map user heap in userspace
    p->mapped_info[HEAP_SEGMENT].va = USER_FREE_ADDRESS_START;
    80000f28:	6c18                	ld	a4,24(s0)
    80000f2a:	fb1c                	sd	a5,48(a4)
    p->mapped_info[HEAP_SEGMENT].npages = 0; // no pages are mapped to heap yet.
    80000f2c:	6c1c                	ld	a5,24(s0)
    80000f2e:	0207ac23          	sw	zero,56(a5) # 400038 <elf_fpread-0x7fbfffc8>
    p->mapped_info[HEAP_SEGMENT].seg_type = HEAP_SEGMENT;
    80000f32:	6c1c                	ld	a5,24(s0)
    80000f34:	470d                	li	a4,3
    80000f36:	dfd8                	sw	a4,60(a5)

    p->total_mapped_region = 4;
    80000f38:	4791                	li	a5,4
    80000f3a:	d01c                	sw	a5,32(s0)
    // sprint("lgm:exec_clean: p->total_mapped_region = %d\n", p->total_mapped_region);
}
    80000f3c:	70a2                	ld	ra,40(sp)
    80000f3e:	7402                	ld	s0,32(sp)
    80000f40:	64e2                	ld	s1,24(sp)
    80000f42:	6942                	ld	s2,16(sp)
    80000f44:	69a2                	ld	s3,8(sp)
    80000f46:	6145                	addi	sp,sp,48
    80000f48:	8082                	ret

0000000080000f4a <fs_init>:
#include "vmm.h"
#include "memlayout.h"
//
// initialize file system
//
void fs_init(void) {
    80000f4a:	1141                	addi	sp,sp,-16
    80000f4c:	e406                	sd	ra,8(sp)
  // initialize the vfs
  vfs_init();
    80000f4e:	693010ef          	jal	80002de0 <vfs_init>

  // register hostfs and mount it as the root
  if( register_hostfs() < 0 ) panic( "fs_init: cannot register hostfs.\n" );
    80000f52:	c72ff0ef          	jal	800003c4 <register_hostfs>
    80000f56:	04054563          	bltz	a0,80000fa0 <fs_init+0x56>
  struct device *hostdev = init_host_device("HOSTDEV");
    80000f5a:	00007517          	auipc	a0,0x7
    80000f5e:	95650513          	addi	a0,a0,-1706 # 800078b0 <rfs_i_ops+0x838>
    80000f62:	cb0ff0ef          	jal	80000412 <init_host_device>
  vfs_mount("HOSTDEV", MOUNT_AS_ROOT);
    80000f66:	4585                	li	a1,1
    80000f68:	00007517          	auipc	a0,0x7
    80000f6c:	94850513          	addi	a0,a0,-1720 # 800078b0 <rfs_i_ops+0x838>
    80000f70:	236020ef          	jal	800031a6 <vfs_mount>

  // register and mount rfs
  if( register_rfs() < 0 ) panic( "fs_init: cannot register rfs.\n" );
    80000f74:	7e6000ef          	jal	8000175a <register_rfs>
    80000f78:	02054a63          	bltz	a0,80000fac <fs_init+0x62>
  struct device *ramdisk0 = init_rfs_device("RAMDISK0");
    80000f7c:	00007517          	auipc	a0,0x7
    80000f80:	95c50513          	addi	a0,a0,-1700 # 800078d8 <rfs_i_ops+0x860>
    80000f84:	5a2000ef          	jal	80001526 <init_rfs_device>
  rfs_format_dev(ramdisk0);
    80000f88:	345000ef          	jal	80001acc <rfs_format_dev>
  vfs_mount("RAMDISK0", MOUNT_DEFAULT);
    80000f8c:	4581                	li	a1,0
    80000f8e:	00007517          	auipc	a0,0x7
    80000f92:	94a50513          	addi	a0,a0,-1718 # 800078d8 <rfs_i_ops+0x860>
    80000f96:	210020ef          	jal	800031a6 <vfs_mount>
}
    80000f9a:	60a2                	ld	ra,8(sp)
    80000f9c:	0141                	addi	sp,sp,16
    80000f9e:	8082                	ret
  if( register_hostfs() < 0 ) panic( "fs_init: cannot register hostfs.\n" );
    80000fa0:	00007517          	auipc	a0,0x7
    80000fa4:	8e850513          	addi	a0,a0,-1816 # 80007888 <rfs_i_ops+0x810>
    80000fa8:	4ef030ef          	jal	80004c96 <do_panic>
  if( register_rfs() < 0 ) panic( "fs_init: cannot register rfs.\n" );
    80000fac:	00007517          	auipc	a0,0x7
    80000fb0:	90c50513          	addi	a0,a0,-1780 # 800078b8 <rfs_i_ops+0x840>
    80000fb4:	4e3030ef          	jal	80004c96 <do_panic>

0000000080000fb8 <init_proc_file_management>:

//
// initialize a proc_file_management data structure for a process.
// return the pointer to the page containing the data structure.
//
proc_file_management *init_proc_file_management(void) {
    80000fb8:	1141                	addi	sp,sp,-16
    80000fba:	e406                	sd	ra,8(sp)
    80000fbc:	e022                	sd	s0,0(sp)
  proc_file_management *pfiles = (proc_file_management *)alloc_page();
    80000fbe:	81fff0ef          	jal	800007dc <alloc_page>
    80000fc2:	842a                	mv	s0,a0
  pfiles->cwd = vfs_root_dentry; // by default, cwd is the root
    80000fc4:	0000f797          	auipc	a5,0xf
    80000fc8:	17c7b783          	ld	a5,380(a5) # 80010140 <vfs_root_dentry>
    80000fcc:	e11c                	sd	a5,0(a0)
  pfiles->nfiles = 0;
    80000fce:	6785                	lui	a5,0x1
    80000fd0:	97aa                	add	a5,a5,a0
    80000fd2:	c007a423          	sw	zero,-1016(a5) # c08 <elf_fpread-0x7ffff3f8>

  for (int fd = 0; fd < MAX_FILES; ++fd)
    80000fd6:	4701                	li	a4,0
    80000fd8:	a809                	j	80000fea <init_proc_file_management+0x32>
    pfiles->opened_files[fd].status = FD_NONE;
    80000fda:	00171793          	slli	a5,a4,0x1
    80000fde:	97ba                	add	a5,a5,a4
    80000fe0:	078e                	slli	a5,a5,0x3
    80000fe2:	97a2                	add	a5,a5,s0
    80000fe4:	0007a423          	sw	zero,8(a5)
  for (int fd = 0; fd < MAX_FILES; ++fd)
    80000fe8:	2705                	addiw	a4,a4,1 # 7fffe001 <elf_fpread-0x1fff>
    80000fea:	07f00793          	li	a5,127
    80000fee:	fee7d6e3          	bge	a5,a4,80000fda <init_proc_file_management+0x22>

  sprint("FS: created a file management struct for a process.\n");
    80000ff2:	00007517          	auipc	a0,0x7
    80000ff6:	8f650513          	addi	a0,a0,-1802 # 800078e8 <rfs_i_ops+0x870>
    80000ffa:	3f9030ef          	jal	80004bf2 <sprint>
  return pfiles;
}
    80000ffe:	8522                	mv	a0,s0
    80001000:	60a2                	ld	ra,8(sp)
    80001002:	6402                	ld	s0,0(sp)
    80001004:	0141                	addi	sp,sp,16
    80001006:	8082                	ret

0000000080001008 <reclaim_proc_file_management>:

//
// reclaim the open-file management data structure of a process.
// note: this function is not used as PKE does not actually reclaim a process.
//
void reclaim_proc_file_management(proc_file_management *pfiles) {
    80001008:	1141                	addi	sp,sp,-16
    8000100a:	e406                	sd	ra,8(sp)
  free_page(pfiles);
    8000100c:	f20ff0ef          	jal	8000072c <free_page>
  return;
}
    80001010:	60a2                	ld	ra,8(sp)
    80001012:	0141                	addi	sp,sp,16
    80001014:	8082                	ret

0000000080001016 <get_opened_file>:

//
// get an opened file from proc->opened_file array.
// return: the pointer to the opened file structure.
//
struct file *get_opened_file(int fd) {
    80001016:	86aa                	mv	a3,a0
  struct file *pfile = NULL;

  // browse opened file list to locate the fd
  for (int i = 0; i < MAX_FILES; ++i) {
    80001018:	4701                	li	a4,0
  struct file *pfile = NULL;
    8000101a:	4501                	li	a0,0
  for (int i = 0; i < MAX_FILES; ++i) {
    8000101c:	07f00793          	li	a5,127
    80001020:	02e7c263          	blt	a5,a4,80001044 <get_opened_file+0x2e>
    pfile = &(current->pfiles->opened_files[i]);  // file entry
    80001024:	0000f797          	auipc	a5,0xf
    80001028:	1047b783          	ld	a5,260(a5) # 80010128 <current>
    8000102c:	1687b503          	ld	a0,360(a5)
    80001030:	00171793          	slli	a5,a4,0x1
    80001034:	97ba                	add	a5,a5,a4
    80001036:	078e                	slli	a5,a5,0x3
    80001038:	953e                	add	a0,a0,a5
    8000103a:	0521                	addi	a0,a0,8
    if (i == fd) break;
    8000103c:	00d70463          	beq	a4,a3,80001044 <get_opened_file+0x2e>
  for (int i = 0; i < MAX_FILES; ++i) {
    80001040:	2705                	addiw	a4,a4,1
    80001042:	bfe9                	j	8000101c <get_opened_file+0x6>
  }
  if (pfile == NULL) panic("do_read: invalid fd!\n");
    80001044:	c111                	beqz	a0,80001048 <get_opened_file+0x32>
  return pfile;
}
    80001046:	8082                	ret
struct file *get_opened_file(int fd) {
    80001048:	1141                	addi	sp,sp,-16
    8000104a:	e406                	sd	ra,8(sp)
  if (pfile == NULL) panic("do_read: invalid fd!\n");
    8000104c:	00007517          	auipc	a0,0x7
    80001050:	8d450513          	addi	a0,a0,-1836 # 80007920 <rfs_i_ops+0x8a8>
    80001054:	443030ef          	jal	80004c96 <do_panic>

0000000080001058 <do_open>:

//
// open a file named as "pathname" with the permission of "flags".
// return: -1 on failure; non-zero file-descriptor on success.
//
int do_open(char *pathname, int flags) {
    80001058:	1101                	addi	sp,sp,-32
    8000105a:	ec06                	sd	ra,24(sp)
    8000105c:	e822                	sd	s0,16(sp)
  struct file *opened_file = NULL;
  if ((opened_file = vfs_open(pathname, flags)) == NULL) return -1;
    8000105e:	442020ef          	jal	800034a0 <vfs_open>
    80001062:	cd35                	beqz	a0,800010de <do_open+0x86>
    80001064:	e426                	sd	s1,8(sp)
    80001066:	85aa                	mv	a1,a0

  int fd = 0;
  if (current->pfiles->nfiles >= MAX_FILES) {
    80001068:	0000f797          	auipc	a5,0xf
    8000106c:	0c07b783          	ld	a5,192(a5) # 80010128 <current>
    80001070:	1687b703          	ld	a4,360(a5)
    80001074:	6785                	lui	a5,0x1
    80001076:	97ba                	add	a5,a5,a4
    80001078:	c087a683          	lw	a3,-1016(a5) # c08 <elf_fpread-0x7ffff3f8>
    8000107c:	07f00793          	li	a5,127
    80001080:	02d7c263          	blt	a5,a3,800010a4 <do_open+0x4c>
    panic("do_open: no file entry for current process!\n");
  }
  struct file *pfile;
  for (fd = 0; fd < MAX_FILES; ++fd) {
    80001084:	4401                	li	s0,0
    80001086:	07f00793          	li	a5,127
    8000108a:	0287c363          	blt	a5,s0,800010b0 <do_open+0x58>
    pfile = &(current->pfiles->opened_files[fd]);
    8000108e:	00141793          	slli	a5,s0,0x1
    80001092:	97a2                	add	a5,a5,s0
    80001094:	078e                	slli	a5,a5,0x3
    80001096:	97ba                	add	a5,a5,a4
    80001098:	00878493          	addi	s1,a5,8
    if (pfile->status == FD_NONE) break;
    8000109c:	479c                	lw	a5,8(a5)
    8000109e:	cb89                	beqz	a5,800010b0 <do_open+0x58>
  for (fd = 0; fd < MAX_FILES; ++fd) {
    800010a0:	2405                	addiw	s0,s0,1
    800010a2:	b7d5                	j	80001086 <do_open+0x2e>
    panic("do_open: no file entry for current process!\n");
    800010a4:	00007517          	auipc	a0,0x7
    800010a8:	89450513          	addi	a0,a0,-1900 # 80007938 <rfs_i_ops+0x8c0>
    800010ac:	3eb030ef          	jal	80004c96 <do_panic>
  }

  // initialize this file structure
  memcpy(pfile, opened_file, sizeof(struct file));
    800010b0:	4661                	li	a2,24
    800010b2:	8526                	mv	a0,s1
    800010b4:	068030ef          	jal	8000411c <memcpy>

  ++current->pfiles->nfiles;
    800010b8:	0000f797          	auipc	a5,0xf
    800010bc:	0707b783          	ld	a5,112(a5) # 80010128 <current>
    800010c0:	1687b783          	ld	a5,360(a5)
    800010c4:	6705                	lui	a4,0x1
    800010c6:	97ba                	add	a5,a5,a4
    800010c8:	c087a703          	lw	a4,-1016(a5)
    800010cc:	2705                	addiw	a4,a4,1 # 1001 <elf_fpread-0x7fffefff>
    800010ce:	c0e7a423          	sw	a4,-1016(a5)
  return fd;
    800010d2:	64a2                	ld	s1,8(sp)
}
    800010d4:	8522                	mv	a0,s0
    800010d6:	60e2                	ld	ra,24(sp)
    800010d8:	6442                	ld	s0,16(sp)
    800010da:	6105                	addi	sp,sp,32
    800010dc:	8082                	ret
  if ((opened_file = vfs_open(pathname, flags)) == NULL) return -1;
    800010de:	547d                	li	s0,-1
    800010e0:	bfd5                	j	800010d4 <do_open+0x7c>

00000000800010e2 <exec_bincode>:
int do_unlink(char *path) {
  return vfs_unlink(path);
}

static void exec_bincode(process *p, char *path)
{
    800010e2:	7131                	addi	sp,sp,-192
    800010e4:	fd06                	sd	ra,184(sp)
    800010e6:	f922                	sd	s0,176(sp)
    800010e8:	f526                	sd	s1,168(sp)
    800010ea:	f14a                	sd	s2,160(sp)
    800010ec:	ed4e                	sd	s3,152(sp)
    800010ee:	e952                	sd	s4,144(sp)
    800010f0:	e556                	sd	s5,136(sp)
    800010f2:	e15a                	sd	s6,128(sp)
    800010f4:	842a                	mv	s0,a0
    800010f6:	84ae                	mv	s1,a1
    sprint("Application: %s\n", path);
    800010f8:	00006517          	auipc	a0,0x6
    800010fc:	21050513          	addi	a0,a0,528 # 80007308 <rfs_i_ops+0x290>
    80001100:	2f3030ef          	jal	80004bf2 <sprint>
    // 加载ehdr
    int fp = do_open(path, O_RDONLY);
    80001104:	4581                	li	a1,0
    80001106:	8526                	mv	a0,s1
    80001108:	f51ff0ef          	jal	80001058 <do_open>
    spike_file_t *f = (spike_file_t *)(get_opened_file(fp)->f_dentry->dentry_inode->i_fs_info); // 看第134行。。。。
    8000110c:	f0bff0ef          	jal	80001016 <get_opened_file>
    80001110:	691c                	ld	a5,16(a0)
    80001112:	779c                	ld	a5,40(a5)
    80001114:	0407b983          	ld	s3,64(a5)
    elf_header ehdr;
    if (spike_file_read(f, &ehdr, sizeof(elf_header)) != sizeof(elf_header))
    80001118:	04000613          	li	a2,64
    8000111c:	008c                	addi	a1,sp,64
    8000111e:	854e                	mv	a0,s3
    80001120:	4c2030ef          	jal	800045e2 <spike_file_read>
    80001124:	04000793          	li	a5,64
    80001128:	00f51c63          	bne	a0,a5,80001140 <exec_bincode+0x5e>
    {
        panic("read elf header error\n");
    }
    if (ehdr.magic != ELF_MAGIC)
    8000112c:	4706                	lw	a4,64(sp)
    8000112e:	464c47b7          	lui	a5,0x464c4
    80001132:	57f78793          	addi	a5,a5,1407 # 464c457f <elf_fpread-0x39b3ba81>
    80001136:	00f71b63          	bne	a4,a5,8000114c <exec_bincode+0x6a>
    }
    // print_ehdr(&ehdr);

    // 加载代码段 & 数据段
    elf_prog_header ph_addr;
    for (int i = 0, off = ehdr.phoff; i < ehdr.phnum; i++, off += sizeof(ph_addr))
    8000113a:	5906                	lw	s2,96(sp)
    8000113c:	4481                	li	s1,0
    8000113e:	a071                	j	800011ca <exec_bincode+0xe8>
        panic("read elf header error\n");
    80001140:	00007517          	auipc	a0,0x7
    80001144:	82850513          	addi	a0,a0,-2008 # 80007968 <rfs_i_ops+0x8f0>
    80001148:	34f030ef          	jal	80004c96 <do_panic>
        panic("do_exec: not an elf file.\n");
    8000114c:	00007517          	auipc	a0,0x7
    80001150:	83450513          	addi	a0,a0,-1996 # 80007980 <rfs_i_ops+0x908>
    80001154:	343030ef          	jal	80004c96 <do_panic>
    {
        // step1: 加载程序头entry
        spike_file_lseek(f, off, SEEK_SET); // seek to the program header
        if (spike_file_read(f, &ph_addr, sizeof(ph_addr)) != sizeof(ph_addr))
        {
            panic("read elf program header error\n");
    80001158:	00007517          	auipc	a0,0x7
    8000115c:	84850513          	addi	a0,a0,-1976 # 800079a0 <rfs_i_ops+0x928>
    80001160:	337030ef          	jal	80004c96 <do_panic>
        }
        if (ph_addr.type != ELF_PROG_LOAD) // 通常只有代码段和数据段是ELF_PROG_LOAD的
            continue;
        if (ph_addr.memsz < ph_addr.filesz)
        {
            panic("memsz < filesz error.\n");
    80001164:	00007517          	auipc	a0,0x7
    80001168:	85c50513          	addi	a0,a0,-1956 # 800079c0 <rfs_i_ops+0x948>
    8000116c:	32b030ef          	jal	80004c96 <do_panic>
        }
        if (ph_addr.vaddr + ph_addr.memsz < ph_addr.vaddr)
        {
            panic("vaddr + memsz < vaddr error.\n");
    80001170:	00007517          	auipc	a0,0x7
    80001174:	86850513          	addi	a0,a0,-1944 # 800079d8 <rfs_i_ops+0x960>
    80001178:	31f030ef          	jal	80004c96 <do_panic>
        memset(pa, 0, PGSIZE);
        user_vm_map((pagetable_t)p->pagetable, ph_addr.vaddr, PGSIZE, (uint64)pa, prot_to_type(PROT_WRITE | PROT_READ | PROT_EXEC, 1));
        spike_file_lseek(f, ph_addr.off, SEEK_SET);
        if (spike_file_read(f, pa, ph_addr.memsz) != ph_addr.memsz)
        {
            panic("read program segment error.\n");
    8000117c:	00007517          	auipc	a0,0x7
    80001180:	87c50513          	addi	a0,a0,-1924 # 800079f8 <rfs_i_ops+0x980>
    80001184:	313030ef          	jal	80004c96 <do_panic>
        int pos;
        for (pos = 0; pos < PGSIZE / sizeof(mapped_region); pos++) // seek the last mapped region
            if (p->mapped_info[pos].va == 0x0)
                break;

        p->mapped_info[pos].va = ph_addr.vaddr;
    80001188:	6c18                	ld	a4,24(s0)
    8000118a:	00459793          	slli	a5,a1,0x4
    8000118e:	973e                	add	a4,a4,a5
    80001190:	66e2                	ld	a3,24(sp)
    80001192:	e314                	sd	a3,0(a4)
        p->mapped_info[pos].npages = 1;
    80001194:	6c18                	ld	a4,24(s0)
    80001196:	973e                	add	a4,a4,a5
    80001198:	4685                	li	a3,1
    8000119a:	c714                	sw	a3,8(a4)

        // SEGMENT_READABLE, SEGMENT_EXECUTABLE, SEGMENT_WRITABLE are defined in kernel/elf.h
        if (ph_addr.flags == (SEGMENT_READABLE | SEGMENT_EXECUTABLE))
    8000119c:	4732                	lw	a4,12(sp)
    8000119e:	4695                	li	a3,5
    800011a0:	0cd70763          	beq	a4,a3,8000126e <exec_bincode+0x18c>
        {
            p->mapped_info[pos].seg_type = CODE_SEGMENT;
            sprint("CODE_SEGMENT added at mapped info offset:%d\n", pos);
        }
        else if (ph_addr.flags == (SEGMENT_READABLE | SEGMENT_WRITABLE))
    800011a4:	4699                	li	a3,6
    800011a6:	0cd71f63          	bne	a4,a3,80001284 <exec_bincode+0x1a2>
        {
            p->mapped_info[pos].seg_type = DATA_SEGMENT;
    800011aa:	6c18                	ld	a4,24(s0)
    800011ac:	97ba                	add	a5,a5,a4
    800011ae:	4715                	li	a4,5
    800011b0:	c7d8                	sw	a4,12(a5)
            sprint("DATA_SEGMENT added at mapped info offset:%d\n", pos);
    800011b2:	00006517          	auipc	a0,0x6
    800011b6:	0be50513          	addi	a0,a0,190 # 80007270 <rfs_i_ops+0x1f8>
    800011ba:	239030ef          	jal	80004bf2 <sprint>
        }
        else
            panic("unknown program segment encountered, segment flag:%d.\n", ph_addr.flags);

        p->total_mapped_region++;
    800011be:	501c                	lw	a5,32(s0)
    800011c0:	2785                	addiw	a5,a5,1
    800011c2:	d01c                	sw	a5,32(s0)
    for (int i = 0, off = ehdr.phoff; i < ehdr.phnum; i++, off += sizeof(ph_addr))
    800011c4:	2485                	addiw	s1,s1,1
    800011c6:	0389091b          	addiw	s2,s2,56
    800011ca:	07815783          	lhu	a5,120(sp)
    800011ce:	0cf4d263          	bge	s1,a5,80001292 <exec_bincode+0x1b0>
        spike_file_lseek(f, off, SEEK_SET); // seek to the program header
    800011d2:	4601                	li	a2,0
    800011d4:	85ca                	mv	a1,s2
    800011d6:	854e                	mv	a0,s3
    800011d8:	42a030ef          	jal	80004602 <spike_file_lseek>
        if (spike_file_read(f, &ph_addr, sizeof(ph_addr)) != sizeof(ph_addr))
    800011dc:	03800613          	li	a2,56
    800011e0:	002c                	addi	a1,sp,8
    800011e2:	854e                	mv	a0,s3
    800011e4:	3fe030ef          	jal	800045e2 <spike_file_read>
    800011e8:	03800793          	li	a5,56
    800011ec:	f6f516e3          	bne	a0,a5,80001158 <exec_bincode+0x76>
        if (ph_addr.type != ELF_PROG_LOAD) // 通常只有代码段和数据段是ELF_PROG_LOAD的
    800011f0:	4722                	lw	a4,8(sp)
    800011f2:	4785                	li	a5,1
    800011f4:	fcf718e3          	bne	a4,a5,800011c4 <exec_bincode+0xe2>
        if (ph_addr.memsz < ph_addr.filesz)
    800011f8:	77c2                	ld	a5,48(sp)
    800011fa:	7722                	ld	a4,40(sp)
    800011fc:	f6e7e4e3          	bltu	a5,a4,80001164 <exec_bincode+0x82>
        if (ph_addr.vaddr + ph_addr.memsz < ph_addr.vaddr)
    80001200:	6762                	ld	a4,24(sp)
    80001202:	97ba                	add	a5,a5,a4
    80001204:	f6e7e6e3          	bltu	a5,a4,80001170 <exec_bincode+0x8e>
        void *pa = alloc_page(); // 分配一页内存
    80001208:	dd4ff0ef          	jal	800007dc <alloc_page>
    8000120c:	8a2a                	mv	s4,a0
        memset(pa, 0, PGSIZE);
    8000120e:	6605                	lui	a2,0x1
    80001210:	4581                	li	a1,0
    80001212:	743020ef          	jal	80004154 <memset>
        user_vm_map((pagetable_t)p->pagetable, ph_addr.vaddr, PGSIZE, (uint64)pa, prot_to_type(PROT_WRITE | PROT_READ | PROT_EXEC, 1));
    80001216:	00843a83          	ld	s5,8(s0)
    8000121a:	6b62                	ld	s6,24(sp)
    8000121c:	4585                	li	a1,1
    8000121e:	451d                	li	a0,7
    80001220:	614020ef          	jal	80003834 <prot_to_type>
    80001224:	0005071b          	sext.w	a4,a0
    80001228:	86d2                	mv	a3,s4
    8000122a:	6605                	lui	a2,0x1
    8000122c:	85da                	mv	a1,s6
    8000122e:	8556                	mv	a0,s5
    80001230:	053020ef          	jal	80003a82 <user_vm_map>
        spike_file_lseek(f, ph_addr.off, SEEK_SET);
    80001234:	4601                	li	a2,0
    80001236:	65c2                	ld	a1,16(sp)
    80001238:	854e                	mv	a0,s3
    8000123a:	3c8030ef          	jal	80004602 <spike_file_lseek>
        if (spike_file_read(f, pa, ph_addr.memsz) != ph_addr.memsz)
    8000123e:	7642                	ld	a2,48(sp)
    80001240:	85d2                	mv	a1,s4
    80001242:	854e                	mv	a0,s3
    80001244:	39e030ef          	jal	800045e2 <spike_file_read>
    80001248:	77c2                	ld	a5,48(sp)
    8000124a:	f2f519e3          	bne	a0,a5,8000117c <exec_bincode+0x9a>
        for (pos = 0; pos < PGSIZE / sizeof(mapped_region); pos++) // seek the last mapped region
    8000124e:	4581                	li	a1,0
    80001250:	0005879b          	sext.w	a5,a1
    80001254:	0ff00713          	li	a4,255
    80001258:	f2f768e3          	bltu	a4,a5,80001188 <exec_bincode+0xa6>
            if (p->mapped_info[pos].va == 0x0)
    8000125c:	6c1c                	ld	a5,24(s0)
    8000125e:	00459713          	slli	a4,a1,0x4
    80001262:	97ba                	add	a5,a5,a4
    80001264:	639c                	ld	a5,0(a5)
    80001266:	f20781e3          	beqz	a5,80001188 <exec_bincode+0xa6>
        for (pos = 0; pos < PGSIZE / sizeof(mapped_region); pos++) // seek the last mapped region
    8000126a:	2585                	addiw	a1,a1,1 # 7fffe001 <elf_fpread-0x1fff>
    8000126c:	b7d5                	j	80001250 <exec_bincode+0x16e>
            p->mapped_info[pos].seg_type = CODE_SEGMENT;
    8000126e:	6c18                	ld	a4,24(s0)
    80001270:	97ba                	add	a5,a5,a4
    80001272:	4711                	li	a4,4
    80001274:	c7d8                	sw	a4,12(a5)
            sprint("CODE_SEGMENT added at mapped info offset:%d\n", pos);
    80001276:	00006517          	auipc	a0,0x6
    8000127a:	fca50513          	addi	a0,a0,-54 # 80007240 <rfs_i_ops+0x1c8>
    8000127e:	175030ef          	jal	80004bf2 <sprint>
    80001282:	bf35                	j	800011be <exec_bincode+0xdc>
            panic("unknown program segment encountered, segment flag:%d.\n", ph_addr.flags);
    80001284:	85ba                	mv	a1,a4
    80001286:	00006517          	auipc	a0,0x6
    8000128a:	01a50513          	addi	a0,a0,26 # 800072a0 <rfs_i_ops+0x228>
    8000128e:	209030ef          	jal	80004c96 <do_panic>
    }
    // 设置tramframe
    p->trapframe->epc = ehdr.entry;
    80001292:	681c                	ld	a5,16(s0)
    80001294:	6766                	ld	a4,88(sp)
    80001296:	10e7b423          	sd	a4,264(a5)
    sprint("Application program entry point (virtual address): 0x%lx\n", p->trapframe->epc);
    8000129a:	681c                	ld	a5,16(s0)
    8000129c:	1087b583          	ld	a1,264(a5)
    800012a0:	00006517          	auipc	a0,0x6
    800012a4:	0f050513          	addi	a0,a0,240 # 80007390 <rfs_i_ops+0x318>
    800012a8:	14b030ef          	jal	80004bf2 <sprint>
}
    800012ac:	70ea                	ld	ra,184(sp)
    800012ae:	744a                	ld	s0,176(sp)
    800012b0:	74aa                	ld	s1,168(sp)
    800012b2:	790a                	ld	s2,160(sp)
    800012b4:	69ea                	ld	s3,152(sp)
    800012b6:	6a4a                	ld	s4,144(sp)
    800012b8:	6aaa                	ld	s5,136(sp)
    800012ba:	6b0a                	ld	s6,128(sp)
    800012bc:	6129                	addi	sp,sp,192
    800012be:	8082                	ret

00000000800012c0 <do_read>:
int do_read(int fd, char *buf, uint64 count) {
    800012c0:	7179                	addi	sp,sp,-48
    800012c2:	f406                	sd	ra,40(sp)
    800012c4:	f022                	sd	s0,32(sp)
    800012c6:	ec26                	sd	s1,24(sp)
    800012c8:	e84a                	sd	s2,16(sp)
    800012ca:	e44e                	sd	s3,8(sp)
    800012cc:	1800                	addi	s0,sp,48
    800012ce:	892e                	mv	s2,a1
    800012d0:	84b2                	mv	s1,a2
  struct file *pfile = get_opened_file(fd);
    800012d2:	d45ff0ef          	jal	80001016 <get_opened_file>
  if (pfile->readable == 0) panic("do_read: no readable file!\n");
    800012d6:	415c                	lw	a5,4(a0)
    800012d8:	cf8d                	beqz	a5,80001312 <do_read+0x52>
  char buffer[count + 1];
    800012da:	01048793          	addi	a5,s1,16
    800012de:	9bc1                	andi	a5,a5,-16
    800012e0:	40f10133          	sub	sp,sp,a5
  int len = vfs_read(pfile, buffer, count);
    800012e4:	8626                	mv	a2,s1
    800012e6:	858a                	mv	a1,sp
    800012e8:	34b010ef          	jal	80002e32 <vfs_read>
    800012ec:	0005099b          	sext.w	s3,a0
  buffer[count] = '\0';
    800012f0:	948a                	add	s1,s1,sp
    800012f2:	00048023          	sb	zero,0(s1)
  strcpy(buf, buffer);
    800012f6:	858a                	mv	a1,sp
    800012f8:	854a                	mv	a0,s2
    800012fa:	6c9020ef          	jal	800041c2 <strcpy>
}
    800012fe:	854e                	mv	a0,s3
    80001300:	fd040113          	addi	sp,s0,-48
    80001304:	70a2                	ld	ra,40(sp)
    80001306:	7402                	ld	s0,32(sp)
    80001308:	64e2                	ld	s1,24(sp)
    8000130a:	6942                	ld	s2,16(sp)
    8000130c:	69a2                	ld	s3,8(sp)
    8000130e:	6145                	addi	sp,sp,48
    80001310:	8082                	ret
  if (pfile->readable == 0) panic("do_read: no readable file!\n");
    80001312:	00006517          	auipc	a0,0x6
    80001316:	70650513          	addi	a0,a0,1798 # 80007a18 <rfs_i_ops+0x9a0>
    8000131a:	17d030ef          	jal	80004c96 <do_panic>

000000008000131e <do_write>:
int do_write(int fd, char *buf, uint64 count) {
    8000131e:	1101                	addi	sp,sp,-32
    80001320:	ec06                	sd	ra,24(sp)
    80001322:	e822                	sd	s0,16(sp)
    80001324:	e426                	sd	s1,8(sp)
    80001326:	842e                	mv	s0,a1
    80001328:	84b2                	mv	s1,a2
  struct file *pfile = get_opened_file(fd);
    8000132a:	cedff0ef          	jal	80001016 <get_opened_file>
  if (pfile->writable == 0) panic("do_write: cannot write file!\n");
    8000132e:	451c                	lw	a5,8(a0)
    80001330:	cb99                	beqz	a5,80001346 <do_write+0x28>
  int len = vfs_write(pfile, buf, count);
    80001332:	8626                	mv	a2,s1
    80001334:	85a2                	mv	a1,s0
    80001336:	33d010ef          	jal	80002e72 <vfs_write>
}
    8000133a:	2501                	sext.w	a0,a0
    8000133c:	60e2                	ld	ra,24(sp)
    8000133e:	6442                	ld	s0,16(sp)
    80001340:	64a2                	ld	s1,8(sp)
    80001342:	6105                	addi	sp,sp,32
    80001344:	8082                	ret
  if (pfile->writable == 0) panic("do_write: cannot write file!\n");
    80001346:	00006517          	auipc	a0,0x6
    8000134a:	6f250513          	addi	a0,a0,1778 # 80007a38 <rfs_i_ops+0x9c0>
    8000134e:	149030ef          	jal	80004c96 <do_panic>

0000000080001352 <do_lseek>:
int do_lseek(int fd, int offset, int whence) {
    80001352:	1101                	addi	sp,sp,-32
    80001354:	ec06                	sd	ra,24(sp)
    80001356:	e822                	sd	s0,16(sp)
    80001358:	e426                	sd	s1,8(sp)
    8000135a:	842e                	mv	s0,a1
    8000135c:	84b2                	mv	s1,a2
  struct file *pfile = get_opened_file(fd);
    8000135e:	cb9ff0ef          	jal	80001016 <get_opened_file>
  return vfs_lseek(pfile, offset, whence);
    80001362:	8626                	mv	a2,s1
    80001364:	85a2                	mv	a1,s0
    80001366:	34d010ef          	jal	80002eb2 <vfs_lseek>
}
    8000136a:	2501                	sext.w	a0,a0
    8000136c:	60e2                	ld	ra,24(sp)
    8000136e:	6442                	ld	s0,16(sp)
    80001370:	64a2                	ld	s1,8(sp)
    80001372:	6105                	addi	sp,sp,32
    80001374:	8082                	ret

0000000080001376 <do_stat>:
int do_stat(int fd, struct istat *istat) {
    80001376:	1141                	addi	sp,sp,-16
    80001378:	e406                	sd	ra,8(sp)
    8000137a:	e022                	sd	s0,0(sp)
    8000137c:	842e                	mv	s0,a1
  struct file *pfile = get_opened_file(fd);
    8000137e:	c99ff0ef          	jal	80001016 <get_opened_file>
  return vfs_stat(pfile, istat);
    80001382:	85a2                	mv	a1,s0
    80001384:	375010ef          	jal	80002ef8 <vfs_stat>
}
    80001388:	60a2                	ld	ra,8(sp)
    8000138a:	6402                	ld	s0,0(sp)
    8000138c:	0141                	addi	sp,sp,16
    8000138e:	8082                	ret

0000000080001390 <do_disk_stat>:
int do_disk_stat(int fd, struct istat *istat) {
    80001390:	1141                	addi	sp,sp,-16
    80001392:	e406                	sd	ra,8(sp)
    80001394:	e022                	sd	s0,0(sp)
    80001396:	842e                	mv	s0,a1
  struct file *pfile = get_opened_file(fd);
    80001398:	c7fff0ef          	jal	80001016 <get_opened_file>
  return vfs_disk_stat(pfile, istat);
    8000139c:	85a2                	mv	a1,s0
    8000139e:	387010ef          	jal	80002f24 <vfs_disk_stat>
}
    800013a2:	60a2                	ld	ra,8(sp)
    800013a4:	6402                	ld	s0,0(sp)
    800013a6:	0141                	addi	sp,sp,16
    800013a8:	8082                	ret

00000000800013aa <do_close>:
int do_close(int fd) {
    800013aa:	1141                	addi	sp,sp,-16
    800013ac:	e406                	sd	ra,8(sp)
  struct file *pfile = get_opened_file(fd);
    800013ae:	c69ff0ef          	jal	80001016 <get_opened_file>
  return vfs_close(pfile);
    800013b2:	3b6020ef          	jal	80003768 <vfs_close>
}
    800013b6:	60a2                	ld	ra,8(sp)
    800013b8:	0141                	addi	sp,sp,16
    800013ba:	8082                	ret

00000000800013bc <do_opendir>:
int do_opendir(char *pathname) {
    800013bc:	1101                	addi	sp,sp,-32
    800013be:	ec06                	sd	ra,24(sp)
    800013c0:	e822                	sd	s0,16(sp)
  if ((opened_file = vfs_opendir(pathname)) == NULL) return -1;
    800013c2:	06a020ef          	jal	8000342c <vfs_opendir>
    800013c6:	c925                	beqz	a0,80001436 <do_opendir+0x7a>
    800013c8:	e426                	sd	s1,8(sp)
    800013ca:	85aa                	mv	a1,a0
  for (fd = 0; fd < MAX_FILES; ++fd) {
    800013cc:	4401                	li	s0,0
    800013ce:	07f00793          	li	a5,127
    800013d2:	0287c363          	blt	a5,s0,800013f8 <do_opendir+0x3c>
    pfile = &(current->pfiles->opened_files[fd]);
    800013d6:	0000f797          	auipc	a5,0xf
    800013da:	d527b783          	ld	a5,-686(a5) # 80010128 <current>
    800013de:	1687b703          	ld	a4,360(a5)
    800013e2:	00141793          	slli	a5,s0,0x1
    800013e6:	97a2                	add	a5,a5,s0
    800013e8:	078e                	slli	a5,a5,0x3
    800013ea:	97ba                	add	a5,a5,a4
    800013ec:	00878493          	addi	s1,a5,8
    if (pfile->status == FD_NONE) break;
    800013f0:	479c                	lw	a5,8(a5)
    800013f2:	c399                	beqz	a5,800013f8 <do_opendir+0x3c>
  for (fd = 0; fd < MAX_FILES; ++fd) {
    800013f4:	2405                	addiw	s0,s0,1
    800013f6:	bfe1                	j	800013ce <do_opendir+0x12>
  if (pfile->status != FD_NONE)  // no free entry
    800013f8:	409c                	lw	a5,0(s1)
    800013fa:	eb85                	bnez	a5,8000142a <do_opendir+0x6e>
  memcpy(pfile, opened_file, sizeof(struct file));
    800013fc:	4661                	li	a2,24
    800013fe:	8526                	mv	a0,s1
    80001400:	51d020ef          	jal	8000411c <memcpy>
  ++current->pfiles->nfiles;
    80001404:	0000f797          	auipc	a5,0xf
    80001408:	d247b783          	ld	a5,-732(a5) # 80010128 <current>
    8000140c:	1687b783          	ld	a5,360(a5)
    80001410:	6705                	lui	a4,0x1
    80001412:	97ba                	add	a5,a5,a4
    80001414:	c087a703          	lw	a4,-1016(a5)
    80001418:	2705                	addiw	a4,a4,1 # 1001 <elf_fpread-0x7fffefff>
    8000141a:	c0e7a423          	sw	a4,-1016(a5)
  return fd;
    8000141e:	64a2                	ld	s1,8(sp)
}
    80001420:	8522                	mv	a0,s0
    80001422:	60e2                	ld	ra,24(sp)
    80001424:	6442                	ld	s0,16(sp)
    80001426:	6105                	addi	sp,sp,32
    80001428:	8082                	ret
    panic("do_opendir: no file entry for current process!\n");
    8000142a:	00006517          	auipc	a0,0x6
    8000142e:	62e50513          	addi	a0,a0,1582 # 80007a58 <rfs_i_ops+0x9e0>
    80001432:	065030ef          	jal	80004c96 <do_panic>
  if ((opened_file = vfs_opendir(pathname)) == NULL) return -1;
    80001436:	547d                	li	s0,-1
    80001438:	b7e5                	j	80001420 <do_opendir+0x64>

000000008000143a <do_readdir>:
int do_readdir(int fd, struct dir *dir) {
    8000143a:	1141                	addi	sp,sp,-16
    8000143c:	e406                	sd	ra,8(sp)
    8000143e:	e022                	sd	s0,0(sp)
    80001440:	842e                	mv	s0,a1
  struct file *pfile = get_opened_file(fd);
    80001442:	bd5ff0ef          	jal	80001016 <get_opened_file>
  return vfs_readdir(pfile, dir);
    80001446:	85a2                	mv	a1,s0
    80001448:	2f1010ef          	jal	80002f38 <vfs_readdir>
}
    8000144c:	60a2                	ld	ra,8(sp)
    8000144e:	6402                	ld	s0,0(sp)
    80001450:	0141                	addi	sp,sp,16
    80001452:	8082                	ret

0000000080001454 <do_mkdir>:
int do_mkdir(char *pathname) {
    80001454:	1141                	addi	sp,sp,-16
    80001456:	e406                	sd	ra,8(sp)
  return vfs_mkdir(pathname);
    80001458:	154020ef          	jal	800035ac <vfs_mkdir>
}
    8000145c:	60a2                	ld	ra,8(sp)
    8000145e:	0141                	addi	sp,sp,16
    80001460:	8082                	ret

0000000080001462 <do_closedir>:
int do_closedir(int fd) {
    80001462:	1141                	addi	sp,sp,-16
    80001464:	e406                	sd	ra,8(sp)
  struct file *pfile = get_opened_file(fd);
    80001466:	bb1ff0ef          	jal	80001016 <get_opened_file>
  return vfs_closedir(pfile);
    8000146a:	2ff010ef          	jal	80002f68 <vfs_closedir>
}
    8000146e:	60a2                	ld	ra,8(sp)
    80001470:	0141                	addi	sp,sp,16
    80001472:	8082                	ret

0000000080001474 <do_link>:
int do_link(char *oldpath, char *newpath) {
    80001474:	1141                	addi	sp,sp,-16
    80001476:	e406                	sd	ra,8(sp)
  return vfs_link(oldpath, newpath);
    80001478:	6f3010ef          	jal	8000336a <vfs_link>
}
    8000147c:	60a2                	ld	ra,8(sp)
    8000147e:	0141                	addi	sp,sp,16
    80001480:	8082                	ret

0000000080001482 <do_unlink>:
int do_unlink(char *path) {
    80001482:	1141                	addi	sp,sp,-16
    80001484:	e406                	sd	ra,8(sp)
  return vfs_unlink(path);
    80001486:	1fe020ef          	jal	80003684 <vfs_unlink>
}
    8000148a:	60a2                	ld	ra,8(sp)
    8000148c:	0141                	addi	sp,sp,16
    8000148e:	8082                	ret

0000000080001490 <do_exec>:


int do_exec(char *path)
{
    80001490:	1101                	addi	sp,sp,-32
    80001492:	ec06                	sd	ra,24(sp)
    80001494:	e822                	sd	s0,16(sp)
    80001496:	e426                	sd	s1,8(sp)
    80001498:	842a                	mv	s0,a0
    exec_clean(current);
    8000149a:	0000f497          	auipc	s1,0xf
    8000149e:	c8e48493          	addi	s1,s1,-882 # 80010128 <current>
    800014a2:	6088                	ld	a0,0(s1)
    800014a4:	98dff0ef          	jal	80000e30 <exec_clean>
    exec_bincode(current, path);
    800014a8:	85a2                	mv	a1,s0
    800014aa:	6088                	ld	a0,0(s1)
    800014ac:	c37ff0ef          	jal	800010e2 <exec_bincode>
    return -1;
    800014b0:	557d                	li	a0,-1
    800014b2:	60e2                	ld	ra,24(sp)
    800014b4:	6442                	ld	s0,16(sp)
    800014b6:	64a2                	ld	s1,8(sp)
    800014b8:	6105                	addi	sp,sp,32
    800014ba:	8082                	ret

00000000800014bc <ramdisk_write>:
struct rfs_device *rfs_device_list[MAX_RAMDISK_COUNT];

//
// write the content stored in "buff" to the "blkno"^th block of disk.
//
int ramdisk_write(struct rfs_device *rfs_device, int blkno){
    800014bc:	1141                	addi	sp,sp,-16
    800014be:	e406                	sd	ra,8(sp)
  if ( blkno < 0 || blkno >= RAMDISK_BLOCK_COUNT )
    800014c0:	0005871b          	sext.w	a4,a1
    800014c4:	07f00793          	li	a5,127
    800014c8:	00e7ef63          	bltu	a5,a4,800014e6 <ramdisk_write+0x2a>
    panic("ramdisk_write: write block No %d out of range!\n", blkno);
  void * dst = (void *)((uint64)rfs_device->d_address + blkno * RAMDISK_BLOCK_SIZE);
    800014cc:	6118                	ld	a4,0(a0)
    800014ce:	00c5979b          	slliw	a5,a1,0xc
  memcpy(dst, rfs_device->iobuffer, RAMDISK_BLOCK_SIZE);
    800014d2:	6605                	lui	a2,0x1
    800014d4:	690c                	ld	a1,16(a0)
    800014d6:	00f70533          	add	a0,a4,a5
    800014da:	443020ef          	jal	8000411c <memcpy>
  return 0;
}
    800014de:	4501                	li	a0,0
    800014e0:	60a2                	ld	ra,8(sp)
    800014e2:	0141                	addi	sp,sp,16
    800014e4:	8082                	ret
    panic("ramdisk_write: write block No %d out of range!\n", blkno);
    800014e6:	00006517          	auipc	a0,0x6
    800014ea:	5aa50513          	addi	a0,a0,1450 # 80007a90 <rfs_i_ops+0xa18>
    800014ee:	7a8030ef          	jal	80004c96 <do_panic>

00000000800014f2 <ramdisk_read>:

//
// read the "blkno"^th block from the RAM disk and store its content into buffer.
//
int ramdisk_read(struct rfs_device *rfs_device, int blkno){
    800014f2:	1141                	addi	sp,sp,-16
    800014f4:	e406                	sd	ra,8(sp)
  if ( blkno < 0 || blkno >= RAMDISK_BLOCK_COUNT )
    800014f6:	0005871b          	sext.w	a4,a1
    800014fa:	07f00793          	li	a5,127
    800014fe:	00e7ee63          	bltu	a5,a4,8000151a <ramdisk_read+0x28>
    panic("ramdisk_read: read block No out of range!\n");
  void * src = (void *)((uint64)rfs_device->d_address + blkno * RAMDISK_BLOCK_SIZE);
    80001502:	611c                	ld	a5,0(a0)
    80001504:	00c5959b          	slliw	a1,a1,0xc
  memcpy(rfs_device->iobuffer, src, RAMDISK_BLOCK_SIZE);
    80001508:	6605                	lui	a2,0x1
    8000150a:	95be                	add	a1,a1,a5
    8000150c:	6908                	ld	a0,16(a0)
    8000150e:	40f020ef          	jal	8000411c <memcpy>
  return 0;
}
    80001512:	4501                	li	a0,0
    80001514:	60a2                	ld	ra,8(sp)
    80001516:	0141                	addi	sp,sp,16
    80001518:	8082                	ret
    panic("ramdisk_read: read block No out of range!\n");
    8000151a:	00006517          	auipc	a0,0x6
    8000151e:	5ae50513          	addi	a0,a0,1454 # 80007ac8 <rfs_i_ops+0xa50>
    80001522:	774030ef          	jal	80004c96 <do_panic>

0000000080001526 <init_rfs_device>:

//
// alloc RAMDISK_BLOCK_COUNT continuous pages (blocks) for the RAM Disk
// setup an vfs node, initialize RAM disk device, and attach the device with the vfs node.
//
struct device *init_rfs_device(const char *dev_name) {
    80001526:	7179                	addi	sp,sp,-48
    80001528:	f406                	sd	ra,40(sp)
    8000152a:	f022                	sd	s0,32(sp)
    8000152c:	ec26                	sd	s1,24(sp)
    8000152e:	e84a                	sd	s2,16(sp)
    80001530:	e44e                	sd	s3,8(sp)
    80001532:	e052                	sd	s4,0(sp)
    80001534:	892a                	mv	s2,a0
  // find rfs in registered fs list
  struct file_system_type *fs_type = NULL;
  for (int i = 0; i < MAX_SUPPORTED_FS; i++) {
    80001536:	4781                	li	a5,0
    80001538:	a011                	j	8000153c <init_rfs_device+0x16>
    8000153a:	2785                	addiw	a5,a5,1
    8000153c:	4725                	li	a4,9
    8000153e:	02f74163          	blt	a4,a5,80001560 <init_rfs_device+0x3a>
    if (fs_list[i] != NULL && fs_list[i]->type_num == RFS_TYPE) {
    80001542:	00379693          	slli	a3,a5,0x3
    80001546:	0000c717          	auipc	a4,0xc
    8000154a:	dba70713          	addi	a4,a4,-582 # 8000d300 <fs_list>
    8000154e:	9736                	add	a4,a4,a3
    80001550:	00073983          	ld	s3,0(a4)
    80001554:	fe0983e3          	beqz	s3,8000153a <init_rfs_device+0x14>
    80001558:	0009a703          	lw	a4,0(s3)
    8000155c:	ff79                	bnez	a4,8000153a <init_rfs_device+0x14>
    8000155e:	a011                	j	80001562 <init_rfs_device+0x3c>
  struct file_system_type *fs_type = NULL;
    80001560:	4981                	li	s3,0
      fs_type = fs_list[i];
      break; 
    }
  }
  if (!fs_type) {
    80001562:	00098563          	beqz	s3,8000156c <init_rfs_device+0x46>

  // alloc blocks for the RAM Disk
  void *curr_addr = NULL;
  void *last_addr = NULL;
  void *ramdisk_addr = NULL;
  for ( int i = 0; i < RAMDISK_BLOCK_COUNT; ++ i ){
    80001566:	4481                	li	s1,0
  void *curr_addr = NULL;
    80001568:	4401                	li	s0,0
    8000156a:	a809                	j	8000157c <init_rfs_device+0x56>
    panic("No RFS file system found!\n");
    8000156c:	00006517          	auipc	a0,0x6
    80001570:	58c50513          	addi	a0,a0,1420 # 80007af8 <rfs_i_ops+0xa80>
    80001574:	722030ef          	jal	80004c96 <do_panic>
  for ( int i = 0; i < RAMDISK_BLOCK_COUNT; ++ i ){
    80001578:	2485                	addiw	s1,s1,1
    last_addr = curr_addr;
    curr_addr = alloc_page();
    8000157a:	842a                	mv	s0,a0
  for ( int i = 0; i < RAMDISK_BLOCK_COUNT; ++ i ){
    8000157c:	07f00793          	li	a5,127
    80001580:	0097cf63          	blt	a5,s1,8000159e <init_rfs_device+0x78>
    curr_addr = alloc_page();
    80001584:	a58ff0ef          	jal	800007dc <alloc_page>
    if ( last_addr != NULL && last_addr - curr_addr != PGSIZE ){
    80001588:	d865                	beqz	s0,80001578 <init_rfs_device+0x52>
    8000158a:	8c09                	sub	s0,s0,a0
    8000158c:	6785                	lui	a5,0x1
    8000158e:	fef405e3          	beq	s0,a5,80001578 <init_rfs_device+0x52>
      panic("RAM Disk0: address is discontinuous!\n");
    80001592:	00006517          	auipc	a0,0x6
    80001596:	58650513          	addi	a0,a0,1414 # 80007b18 <rfs_i_ops+0xaa0>
    8000159a:	6fc030ef          	jal	80004c96 <do_panic>
  ramdisk_addr = curr_addr;

  // find a free rfs device
  struct rfs_device **rfs_device = NULL;
  int device_id = 0;
  for (int i = 0; i < MAX_RAMDISK_COUNT; i++) {
    8000159e:	4481                	li	s1,0
    800015a0:	47a5                	li	a5,9
    800015a2:	0897cd63          	blt	a5,s1,8000163c <init_rfs_device+0x116>
    if (rfs_device_list[i] == NULL) {
    800015a6:	00349713          	slli	a4,s1,0x3
    800015aa:	0000c797          	auipc	a5,0xc
    800015ae:	c6678793          	addi	a5,a5,-922 # 8000d210 <rfs_device_list>
    800015b2:	97ba                	add	a5,a5,a4
    800015b4:	639c                	ld	a5,0(a5)
    800015b6:	c399                	beqz	a5,800015bc <init_rfs_device+0x96>
  for (int i = 0; i < MAX_RAMDISK_COUNT; i++) {
    800015b8:	2485                	addiw	s1,s1,1
    800015ba:	b7dd                	j	800015a0 <init_rfs_device+0x7a>
      rfs_device = &rfs_device_list[i];
    800015bc:	0000ca17          	auipc	s4,0xc
    800015c0:	c54a0a13          	addi	s4,s4,-940 # 8000d210 <rfs_device_list>
    800015c4:	9a3a                	add	s4,s4,a4
  }
  if (!rfs_device) {
    panic("RAM Disk0: no free device!\n");
  }
  
  *rfs_device = (struct rfs_device *)alloc_page();
    800015c6:	a16ff0ef          	jal	800007dc <alloc_page>
    800015ca:	00aa3023          	sd	a0,0(s4)
  (*rfs_device)->d_blocks = RAMDISK_BLOCK_COUNT;
    800015ce:	08000793          	li	a5,128
    800015d2:	c51c                	sw	a5,8(a0)
  (*rfs_device)->d_blocksize = RAMDISK_BLOCK_SIZE;
    800015d4:	000a3783          	ld	a5,0(s4)
    800015d8:	6705                	lui	a4,0x1
    800015da:	c7d8                	sw	a4,12(a5)
  (*rfs_device)->d_write = ramdisk_write;
    800015dc:	000a3783          	ld	a5,0(s4)
    800015e0:	00000717          	auipc	a4,0x0
    800015e4:	edc70713          	addi	a4,a4,-292 # 800014bc <ramdisk_write>
    800015e8:	ef98                	sd	a4,24(a5)
  (*rfs_device)->d_read = ramdisk_read;
    800015ea:	000a3783          	ld	a5,0(s4)
    800015ee:	00000717          	auipc	a4,0x0
    800015f2:	f0470713          	addi	a4,a4,-252 # 800014f2 <ramdisk_read>
    800015f6:	f398                	sd	a4,32(a5)
  (*rfs_device)->d_address = ramdisk_addr;
    800015f8:	000a3783          	ld	a5,0(s4)
    800015fc:	e380                	sd	s0,0(a5)
  (*rfs_device)->iobuffer = alloc_page();
    800015fe:	000a3a03          	ld	s4,0(s4)
    80001602:	9daff0ef          	jal	800007dc <alloc_page>
    80001606:	00aa3823          	sd	a0,16(s4)

  // allocate a vfs device
  struct device * device = (struct device *)alloc_page();
    8000160a:	9d2ff0ef          	jal	800007dc <alloc_page>
    8000160e:	8a2a                	mv	s4,a0
  // set the device name and index
  strcpy(device->dev_name, dev_name);
    80001610:	85ca                	mv	a1,s2
    80001612:	3b1020ef          	jal	800041c2 <strcpy>
  device->dev_id = device_id;
    80001616:	029a2023          	sw	s1,32(s4)
  device->fs_type = fs_type;
    8000161a:	033a3423          	sd	s3,40(s4)

  // add the device to the vfs device list
  for(int i = 0; i < MAX_VFS_DEV; i++) {
    8000161e:	4781                	li	a5,0
    80001620:	4725                	li	a4,9
    80001622:	02f74b63          	blt	a4,a5,80001658 <init_rfs_device+0x132>
    if (vfs_dev_list[i] == NULL) {
    80001626:	00379693          	slli	a3,a5,0x3
    8000162a:	0000c717          	auipc	a4,0xc
    8000162e:	c3670713          	addi	a4,a4,-970 # 8000d260 <vfs_dev_list>
    80001632:	9736                	add	a4,a4,a3
    80001634:	6318                	ld	a4,0(a4)
    80001636:	cb09                	beqz	a4,80001648 <init_rfs_device+0x122>
  for(int i = 0; i < MAX_VFS_DEV; i++) {
    80001638:	2785                	addiw	a5,a5,1
    8000163a:	b7dd                	j	80001620 <init_rfs_device+0xfa>
    panic("RAM Disk0: no free device!\n");
    8000163c:	00006517          	auipc	a0,0x6
    80001640:	50450513          	addi	a0,a0,1284 # 80007b40 <rfs_i_ops+0xac8>
    80001644:	652030ef          	jal	80004c96 <do_panic>
      vfs_dev_list[i] = device;
    80001648:	0000c717          	auipc	a4,0xc
    8000164c:	c1870713          	addi	a4,a4,-1000 # 8000d260 <vfs_dev_list>
    80001650:	00d707b3          	add	a5,a4,a3
    80001654:	0147b023          	sd	s4,0(a5)
      break;
    }
  }

  sprint("%s: base address of %s is: %p\n",dev_name, dev_name, ramdisk_addr);
    80001658:	86a2                	mv	a3,s0
    8000165a:	864a                	mv	a2,s2
    8000165c:	85ca                	mv	a1,s2
    8000165e:	00006517          	auipc	a0,0x6
    80001662:	50250513          	addi	a0,a0,1282 # 80007b60 <rfs_i_ops+0xae8>
    80001666:	58c030ef          	jal	80004bf2 <sprint>
  return device;
    8000166a:	8552                	mv	a0,s4
    8000166c:	70a2                	ld	ra,40(sp)
    8000166e:	7402                	ld	s0,32(sp)
    80001670:	64e2                	ld	s1,24(sp)
    80001672:	6942                	ld	s2,16(sp)
    80001674:	69a2                	ld	s3,8(sp)
    80001676:	6a02                	ld	s4,0(sp)
    80001678:	6145                	addi	sp,sp,48
    8000167a:	8082                	ret

000000008000167c <rfs_lseek>:
// there are two types of seek (specify by whence): LSEEK_SET, SEEK_CUR
// LSEEK_SET: set the file pointer to the offset
// LSEEK_CUR: set the file pointer to the current offset plus the offset
// return 0 if success, otherwise return -1
//
int rfs_lseek(struct vinode *f_inode, ssize_t new_offset, int whence, int *offset) {
    8000167c:	1141                	addi	sp,sp,-16
    8000167e:	e406                	sd	ra,8(sp)
  int file_size = f_inode->size;
    80001680:	451c                	lw	a5,8(a0)

  switch (whence) {
    80001682:	ce11                	beqz	a2,8000169e <rfs_lseek+0x22>
    80001684:	4705                	li	a4,1
    80001686:	02e60b63          	beq	a2,a4,800016bc <rfs_lseek+0x40>
        return -1;
      }
      *offset += new_offset;
      break;
    default:
      sprint("rfs_lseek: invalid whence!\n");
    8000168a:	00006517          	auipc	a0,0x6
    8000168e:	51650513          	addi	a0,a0,1302 # 80007ba0 <rfs_i_ops+0xb28>
    80001692:	560030ef          	jal	80004bf2 <sprint>
      return -1;
    80001696:	557d                	li	a0,-1
  }
  
  return 0;
}
    80001698:	60a2                	ld	ra,8(sp)
    8000169a:	0141                	addi	sp,sp,16
    8000169c:	8082                	ret
      if (new_offset < 0 || new_offset > file_size) {
    8000169e:	0005c763          	bltz	a1,800016ac <rfs_lseek+0x30>
    800016a2:	00b7c563          	blt	a5,a1,800016ac <rfs_lseek+0x30>
      *offset = new_offset;
    800016a6:	c28c                	sw	a1,0(a3)
  return 0;
    800016a8:	8532                	mv	a0,a2
      break;
    800016aa:	b7fd                	j	80001698 <rfs_lseek+0x1c>
        sprint("rfs_lseek: invalid offset!\n");
    800016ac:	00006517          	auipc	a0,0x6
    800016b0:	4d450513          	addi	a0,a0,1236 # 80007b80 <rfs_i_ops+0xb08>
    800016b4:	53e030ef          	jal	80004bf2 <sprint>
        return -1;
    800016b8:	557d                	li	a0,-1
    800016ba:	bff9                	j	80001698 <rfs_lseek+0x1c>
      if (*offset + new_offset < 0 || *offset + new_offset > file_size) {
    800016bc:	4290                	lw	a2,0(a3)
    800016be:	00b60733          	add	a4,a2,a1
    800016c2:	00074863          	bltz	a4,800016d2 <rfs_lseek+0x56>
    800016c6:	00e7c663          	blt	a5,a4,800016d2 <rfs_lseek+0x56>
      *offset += new_offset;
    800016ca:	9db1                	addw	a1,a1,a2
    800016cc:	c28c                	sw	a1,0(a3)
  return 0;
    800016ce:	4501                	li	a0,0
      break;
    800016d0:	b7e1                	j	80001698 <rfs_lseek+0x1c>
        sprint("rfs_lseek: invalid offset!\n");
    800016d2:	00006517          	auipc	a0,0x6
    800016d6:	4ae50513          	addi	a0,a0,1198 # 80007b80 <rfs_i_ops+0xb08>
    800016da:	518030ef          	jal	80004bf2 <sprint>
        return -1;
    800016de:	557d                	li	a0,-1
    800016e0:	bf65                	j	80001698 <rfs_lseek+0x1c>

00000000800016e2 <rfs_readdir>:
// the position of the entry to be read. if offset is 0, the first entry is read,
// if offset is 1, the second entry is read, and so on.
// return: 0 on success, -1 when there are no more entry (end of the list).
//
int rfs_readdir(struct vinode *dir_vinode, struct dir *dir, int *offset) {
  int total_direntrys = dir_vinode->size / sizeof(struct rfs_direntry);
    800016e2:	4518                	lw	a4,8(a0)
    800016e4:	8715                	srai	a4,a4,0x5
  int one_block_direntrys = RFS_BLKSIZE / sizeof(struct rfs_direntry);

  int direntry_index = *offset;
    800016e6:	421c                	lw	a5,0(a2)
  if (direntry_index >= total_direntrys) {
    800016e8:	04e7d063          	bge	a5,a4,80001728 <rfs_readdir+0x46>
int rfs_readdir(struct vinode *dir_vinode, struct dir *dir, int *offset) {
    800016ec:	1101                	addi	sp,sp,-32
    800016ee:	ec06                	sd	ra,24(sp)
    800016f0:	e822                	sd	s0,16(sp)
    800016f2:	e426                	sd	s1,8(sp)
    800016f4:	e04a                	sd	s2,0(sp)
    800016f6:	84ae                	mv	s1,a1
    800016f8:	8432                	mv	s0,a2
    // no more direntry
    return -1;
  }

  // reads a directory entry from the directory cache stored in vfs inode.
  struct rfs_dir_cache *dir_cache =
    800016fa:	6138                	ld	a4,64(a0)
      (struct rfs_dir_cache *)dir_vinode->i_fs_info;
  struct rfs_direntry *p_direntry = dir_cache->dir_base_addr + direntry_index;
    800016fc:	00873903          	ld	s2,8(a4)
    80001700:	0796                	slli	a5,a5,0x5
    80001702:	993e                	add	s2,s2,a5
  // *offset, and used p_direntry to point it.
  // in the remaining processing, we need to return our discovery.
  // the method of returning is to popular proper members of "dir", more specifically,
  // dir->name and dir->inum.
  // note: DO NOT DELETE CODE BELOW PANIC.
	strcpy(dir->name, p_direntry->name);
    80001704:	00490593          	addi	a1,s2,4
    80001708:	8526                	mv	a0,s1
    8000170a:	2b9020ef          	jal	800041c2 <strcpy>
	dir->inum = p_direntry->inum;
    8000170e:	00092783          	lw	a5,0(s2)
    80001712:	d09c                	sw	a5,32(s1)

  // DO NOT DELETE CODE BELOW.
  (*offset)++;
    80001714:	401c                	lw	a5,0(s0)
    80001716:	2785                	addiw	a5,a5,1
    80001718:	c01c                	sw	a5,0(s0)
  return 0;
    8000171a:	4501                	li	a0,0
}
    8000171c:	60e2                	ld	ra,24(sp)
    8000171e:	6442                	ld	s0,16(sp)
    80001720:	64a2                	ld	s1,8(sp)
    80001722:	6902                	ld	s2,0(sp)
    80001724:	6105                	addi	sp,sp,32
    80001726:	8082                	ret
    return -1;
    80001728:	557d                	li	a0,-1
}
    8000172a:	8082                	ret

000000008000172c <rfs_hook_closedir>:
int rfs_hook_closedir(struct vinode *dir_vinode, struct dentry *dentry) {
    8000172c:	1101                	addi	sp,sp,-32
    8000172e:	ec06                	sd	ra,24(sp)
    80001730:	e822                	sd	s0,16(sp)
    80001732:	e426                	sd	s1,8(sp)
  struct rfs_dir_cache *dir_cache =
    80001734:	6124                	ld	s1,64(a0)
  for (int i = 0; i < dir_cache->block_count; ++i) {
    80001736:	4401                	li	s0,0
    80001738:	a801                	j	80001748 <rfs_hook_closedir+0x1c>
    free_page((char *)dir_cache->dir_base_addr + i * RFS_BLKSIZE);
    8000173a:	6488                	ld	a0,8(s1)
    8000173c:	00c4179b          	slliw	a5,s0,0xc
    80001740:	953e                	add	a0,a0,a5
    80001742:	febfe0ef          	jal	8000072c <free_page>
  for (int i = 0; i < dir_cache->block_count; ++i) {
    80001746:	2405                	addiw	s0,s0,1
    80001748:	409c                	lw	a5,0(s1)
    8000174a:	fef448e3          	blt	s0,a5,8000173a <rfs_hook_closedir+0xe>
}
    8000174e:	4501                	li	a0,0
    80001750:	60e2                	ld	ra,24(sp)
    80001752:	6442                	ld	s0,16(sp)
    80001754:	64a2                	ld	s1,8(sp)
    80001756:	6105                	addi	sp,sp,32
    80001758:	8082                	ret

000000008000175a <register_rfs>:
int register_rfs() {
    8000175a:	1141                	addi	sp,sp,-16
    8000175c:	e406                	sd	ra,8(sp)
  struct file_system_type *fs_type = (struct file_system_type *)alloc_page();
    8000175e:	87eff0ef          	jal	800007dc <alloc_page>
  fs_type->type_num = RFS_TYPE;
    80001762:	00052023          	sw	zero,0(a0)
  fs_type->get_superblock = rfs_get_superblock;
    80001766:	00001797          	auipc	a5,0x1
    8000176a:	d2a78793          	addi	a5,a5,-726 # 80002490 <rfs_get_superblock>
    8000176e:	e51c                	sd	a5,8(a0)
  for (int i = 0; i < MAX_SUPPORTED_FS; i++) {
    80001770:	4781                	li	a5,0
    80001772:	4725                	li	a4,9
    80001774:	02f74663          	blt	a4,a5,800017a0 <register_rfs+0x46>
    if (fs_list[i] == NULL) {
    80001778:	00379693          	slli	a3,a5,0x3
    8000177c:	0000c717          	auipc	a4,0xc
    80001780:	b8470713          	addi	a4,a4,-1148 # 8000d300 <fs_list>
    80001784:	9736                	add	a4,a4,a3
    80001786:	6318                	ld	a4,0(a4)
    80001788:	c319                	beqz	a4,8000178e <register_rfs+0x34>
  for (int i = 0; i < MAX_SUPPORTED_FS; i++) {
    8000178a:	2785                	addiw	a5,a5,1
    8000178c:	b7dd                	j	80001772 <register_rfs+0x18>
      fs_list[i] = fs_type;
    8000178e:	0000c717          	auipc	a4,0xc
    80001792:	b7270713          	addi	a4,a4,-1166 # 8000d300 <fs_list>
    80001796:	00d707b3          	add	a5,a4,a3
    8000179a:	e388                	sd	a0,0(a5)
      return 0;
    8000179c:	4501                	li	a0,0
    8000179e:	a011                	j	800017a2 <register_rfs+0x48>
  return -1;
    800017a0:	557d                	li	a0,-1
}
    800017a2:	60a2                	ld	ra,8(sp)
    800017a4:	0141                	addi	sp,sp,16
    800017a6:	8082                	ret

00000000800017a8 <rfs_r1block>:
int rfs_r1block(struct rfs_device *rfs_dev, int n_block) {
    800017a8:	1141                	addi	sp,sp,-16
    800017aa:	e406                	sd	ra,8(sp)
  return dop_read(rfs_dev, n_block);
    800017ac:	711c                	ld	a5,32(a0)
    800017ae:	9782                	jalr	a5
}
    800017b0:	60a2                	ld	ra,8(sp)
    800017b2:	0141                	addi	sp,sp,16
    800017b4:	8082                	ret

00000000800017b6 <rfs_read>:
                 int *offset) {
    800017b6:	7159                	addi	sp,sp,-112
    800017b8:	f486                	sd	ra,104(sp)
    800017ba:	f0a2                	sd	s0,96(sp)
    800017bc:	eca6                	sd	s1,88(sp)
    800017be:	e8ca                	sd	s2,80(sp)
    800017c0:	e4ce                	sd	s3,72(sp)
    800017c2:	e0d2                	sd	s4,64(sp)
    800017c4:	fc56                	sd	s5,56(sp)
    800017c6:	f85a                	sd	s6,48(sp)
    800017c8:	f45e                	sd	s7,40(sp)
    800017ca:	f062                	sd	s8,32(sp)
    800017cc:	ec66                	sd	s9,24(sp)
    800017ce:	e86a                	sd	s10,16(sp)
    800017d0:	e46e                	sd	s11,8(sp)
    800017d2:	1880                	addi	s0,sp,112
  if (f_inode->size < *offset)
    800017d4:	451c                	lw	a5,8(a0)
    800017d6:	4284                	lw	s1,0(a3)
    800017d8:	0e97c663          	blt	a5,s1,800018c4 <rfs_read+0x10e>
    800017dc:	8aaa                	mv	s5,a0
    800017de:	8d2e                	mv	s10,a1
    800017e0:	8b32                	mv	s6,a2
    800017e2:	8cb6                	mv	s9,a3
  if (f_inode->size < (*offset + len)) len = f_inode->size - *offset;
    800017e4:	00c48733          	add	a4,s1,a2
    800017e8:	00e7d463          	bge	a5,a4,800017f0 <rfs_read+0x3a>
    800017ec:	40978b3b          	subw	s6,a5,s1
  char buffer[len + 1];
    800017f0:	010b0793          	addi	a5,s6,16
    800017f4:	9bc1                	andi	a5,a5,-16
    800017f6:	40f10133          	sub	sp,sp,a5
    800017fa:	8c0a                	mv	s8,sp
  int align = *offset % RFS_BLKSIZE;
    800017fc:	41f4d79b          	sraiw	a5,s1,0x1f
    80001800:	0147d79b          	srliw	a5,a5,0x14
    80001804:	9cbd                	addw	s1,s1,a5
    80001806:	6685                	lui	a3,0x1
    80001808:	16fd                	addi	a3,a3,-1 # fff <elf_fpread-0x7ffff001>
    8000180a:	00d4f9b3          	and	s3,s1,a3
    8000180e:	40f989bb          	subw	s3,s3,a5
    80001812:	00098d9b          	sext.w	s11,s3
  int block_offset = *offset / RFS_BLKSIZE;
    80001816:	40c4d49b          	sraiw	s1,s1,0xc
    8000181a:	0004879b          	sext.w	a5,s1
  int readtimes = (align + len) / RFS_BLKSIZE;
    8000181e:	016d8bb3          	add	s7,s11,s6
    80001822:	43fbd713          	srai	a4,s7,0x3f
    80001826:	00d77933          	and	s2,a4,a3
    8000182a:	995e                	add	s2,s2,s7
    8000182c:	40c95913          	srai	s2,s2,0xc
    80001830:	2901                	sext.w	s2,s2
  int remain = (align + len) % RFS_BLKSIZE;
    80001832:	9351                	srli	a4,a4,0x34
    80001834:	9bba                	add	s7,s7,a4
    80001836:	00dbfbb3          	and	s7,s7,a3
    8000183a:	40eb8bbb          	subw	s7,s7,a4
  struct rfs_device *rdev = rfs_device_list[f_inode->sb->s_dev->dev_id];
    8000183e:	048ab703          	ld	a4,72(s5)
    80001842:	6f18                	ld	a4,24(a4)
    80001844:	5314                	lw	a3,32(a4)
    80001846:	068e                	slli	a3,a3,0x3
    80001848:	0000c717          	auipc	a4,0xc
    8000184c:	9c870713          	addi	a4,a4,-1592 # 8000d210 <rfs_device_list>
    80001850:	9736                	add	a4,a4,a3
    80001852:	00073a03          	ld	s4,0(a4)
  rfs_r1block(rdev, f_inode->addrs[block_offset]);
    80001856:	0791                	addi	a5,a5,4
    80001858:	078a                	slli	a5,a5,0x2
    8000185a:	97d6                	add	a5,a5,s5
    8000185c:	478c                	lw	a1,8(a5)
    8000185e:	8552                	mv	a0,s4
    80001860:	f49ff0ef          	jal	800017a8 <rfs_r1block>
  int first_block_len = (readtimes == 0 ? len : RFS_BLKSIZE - align);
    80001864:	06090663          	beqz	s2,800018d0 <rfs_read+0x11a>
    80001868:	6785                	lui	a5,0x1
    8000186a:	413789bb          	subw	s3,a5,s3
  memcpy(buffer + buf_offset, rdev->iobuffer + align, first_block_len);
    8000186e:	010a3583          	ld	a1,16(s4)
    80001872:	864e                	mv	a2,s3
    80001874:	95ee                	add	a1,a1,s11
    80001876:	8562                	mv	a0,s8
    80001878:	0a5020ef          	jal	8000411c <memcpy>
  block_offset++;
    8000187c:	2485                	addiw	s1,s1,1
  readtimes--;
    8000187e:	397d                	addiw	s2,s2,-1
  if (readtimes >= 0) {
    80001880:	06095f63          	bgez	s2,800018fe <rfs_read+0x148>
  buffer[len] = '\0';
    80001884:	016c07b3          	add	a5,s8,s6
    80001888:	00078023          	sb	zero,0(a5) # 1000 <elf_fpread-0x7ffff000>
  strcpy(r_buf, buffer);
    8000188c:	85e2                	mv	a1,s8
    8000188e:	856a                	mv	a0,s10
    80001890:	133020ef          	jal	800041c2 <strcpy>
  *offset += len;
    80001894:	000ca783          	lw	a5,0(s9)
    80001898:	00fb07bb          	addw	a5,s6,a5
    8000189c:	00fca023          	sw	a5,0(s9)
}
    800018a0:	855a                	mv	a0,s6
    800018a2:	f9040113          	addi	sp,s0,-112
    800018a6:	70a6                	ld	ra,104(sp)
    800018a8:	7406                	ld	s0,96(sp)
    800018aa:	64e6                	ld	s1,88(sp)
    800018ac:	6946                	ld	s2,80(sp)
    800018ae:	69a6                	ld	s3,72(sp)
    800018b0:	6a06                	ld	s4,64(sp)
    800018b2:	7ae2                	ld	s5,56(sp)
    800018b4:	7b42                	ld	s6,48(sp)
    800018b6:	7ba2                	ld	s7,40(sp)
    800018b8:	7c02                	ld	s8,32(sp)
    800018ba:	6ce2                	ld	s9,24(sp)
    800018bc:	6d42                	ld	s10,16(sp)
    800018be:	6da2                	ld	s11,8(sp)
    800018c0:	6165                	addi	sp,sp,112
    800018c2:	8082                	ret
    panic("rfs_read:offset should less than file size!");
    800018c4:	00006517          	auipc	a0,0x6
    800018c8:	2fc50513          	addi	a0,a0,764 # 80007bc0 <rfs_i_ops+0xb48>
    800018cc:	3ca030ef          	jal	80004c96 <do_panic>
  int first_block_len = (readtimes == 0 ? len : RFS_BLKSIZE - align);
    800018d0:	000b099b          	sext.w	s3,s6
    800018d4:	bf69                	j	8000186e <rfs_read+0xb8>
      rfs_r1block(rdev, f_inode->addrs[block_offset]);
    800018d6:	00448793          	addi	a5,s1,4
    800018da:	078a                	slli	a5,a5,0x2
    800018dc:	97d6                	add	a5,a5,s5
    800018de:	478c                	lw	a1,8(a5)
    800018e0:	8552                	mv	a0,s4
    800018e2:	ec7ff0ef          	jal	800017a8 <rfs_r1block>
      memcpy(buffer + buf_offset, rdev->iobuffer, RFS_BLKSIZE);
    800018e6:	6605                	lui	a2,0x1
    800018e8:	010a3583          	ld	a1,16(s4)
    800018ec:	013c0533          	add	a0,s8,s3
    800018f0:	02d020ef          	jal	8000411c <memcpy>
      buf_offset += RFS_BLKSIZE;
    800018f4:	6785                	lui	a5,0x1
    800018f6:	013789bb          	addw	s3,a5,s3
      block_offset++;
    800018fa:	2485                	addiw	s1,s1,1
      readtimes--;
    800018fc:	397d                	addiw	s2,s2,-1
    while (readtimes != 0) {
    800018fe:	fc091ce3          	bnez	s2,800018d6 <rfs_read+0x120>
    if (remain > 0) {
    80001902:	f97051e3          	blez	s7,80001884 <rfs_read+0xce>
      rfs_r1block(rdev, f_inode->addrs[block_offset]);
    80001906:	0491                	addi	s1,s1,4
    80001908:	048a                	slli	s1,s1,0x2
    8000190a:	9aa6                	add	s5,s5,s1
    8000190c:	008aa583          	lw	a1,8(s5)
    80001910:	8552                	mv	a0,s4
    80001912:	e97ff0ef          	jal	800017a8 <rfs_r1block>
      memcpy(buffer + buf_offset, rdev->iobuffer, remain);
    80001916:	865e                	mv	a2,s7
    80001918:	010a3583          	ld	a1,16(s4)
    8000191c:	013c0533          	add	a0,s8,s3
    80001920:	7fc020ef          	jal	8000411c <memcpy>
    80001924:	b785                	j	80001884 <rfs_read+0xce>

0000000080001926 <rfs_hook_opendir>:
int rfs_hook_opendir(struct vinode *dir_vinode, struct dentry *dentry) {
    80001926:	7179                	addi	sp,sp,-48
    80001928:	f406                	sd	ra,40(sp)
    8000192a:	f022                	sd	s0,32(sp)
    8000192c:	ec26                	sd	s1,24(sp)
    8000192e:	e84a                	sd	s2,16(sp)
    80001930:	e44e                	sd	s3,8(sp)
    80001932:	e052                	sd	s4,0(sp)
    80001934:	8a2a                	mv	s4,a0
  struct rfs_device *rdev = rfs_device_list[dir_vinode->sb->s_dev->dev_id];
    80001936:	653c                	ld	a5,72(a0)
    80001938:	6f9c                	ld	a5,24(a5)
    8000193a:	5398                	lw	a4,32(a5)
    8000193c:	070e                	slli	a4,a4,0x3
    8000193e:	0000c797          	auipc	a5,0xc
    80001942:	8d278793          	addi	a5,a5,-1838 # 8000d210 <rfs_device_list>
    80001946:	97ba                	add	a5,a5,a4
    80001948:	0007b983          	ld	s3,0(a5)
  for (int i = dir_vinode->blocks - 1; i >= 0; i--) {
    8000194c:	01452903          	lw	s2,20(a0)
    80001950:	397d                	addiw	s2,s2,-1
  void *pdire = NULL;
    80001952:	4481                	li	s1,0
  for (int i = dir_vinode->blocks - 1; i >= 0; i--) {
    80001954:	a00d                	j	80001976 <rfs_hook_opendir+0x50>
    rfs_r1block(rdev, dir_vinode->addrs[i]);
    80001956:	00490793          	addi	a5,s2,4
    8000195a:	078a                	slli	a5,a5,0x2
    8000195c:	97d2                	add	a5,a5,s4
    8000195e:	478c                	lw	a1,8(a5)
    80001960:	854e                	mv	a0,s3
    80001962:	e47ff0ef          	jal	800017a8 <rfs_r1block>
    memcpy(pdire, rdev->iobuffer, RFS_BLKSIZE);
    80001966:	6605                	lui	a2,0x1
    80001968:	0109b583          	ld	a1,16(s3)
    8000196c:	8522                	mv	a0,s0
    8000196e:	7ae020ef          	jal	8000411c <memcpy>
  for (int i = dir_vinode->blocks - 1; i >= 0; i--) {
    80001972:	397d                	addiw	s2,s2,-1
    pdire = alloc_page();
    80001974:	84a2                	mv	s1,s0
  for (int i = dir_vinode->blocks - 1; i >= 0; i--) {
    80001976:	02094063          	bltz	s2,80001996 <rfs_hook_opendir+0x70>
    pdire = alloc_page();
    8000197a:	e63fe0ef          	jal	800007dc <alloc_page>
    8000197e:	842a                	mv	s0,a0
    if (previous != NULL && previous - pdire != RFS_BLKSIZE)
    80001980:	d8f9                	beqz	s1,80001956 <rfs_hook_opendir+0x30>
    80001982:	8c89                	sub	s1,s1,a0
    80001984:	6785                	lui	a5,0x1
    80001986:	fcf488e3          	beq	s1,a5,80001956 <rfs_hook_opendir+0x30>
      panic("rfs_hook_opendir: memory discontinuity");
    8000198a:	00006517          	auipc	a0,0x6
    8000198e:	26650513          	addi	a0,a0,614 # 80007bf0 <rfs_i_ops+0xb78>
    80001992:	304030ef          	jal	80004c96 <do_panic>
  struct rfs_dir_cache *dir_cache = (struct rfs_dir_cache *)alloc_page();
    80001996:	e47fe0ef          	jal	800007dc <alloc_page>
  dir_cache->block_count = dir_vinode->blocks;
    8000199a:	014a2783          	lw	a5,20(s4)
    8000199e:	c11c                	sw	a5,0(a0)
  dir_cache->dir_base_addr = (struct rfs_direntry *)pdire;
    800019a0:	e504                	sd	s1,8(a0)
  dir_vinode->i_fs_info = dir_cache;
    800019a2:	04aa3023          	sd	a0,64(s4)
}
    800019a6:	4501                	li	a0,0
    800019a8:	70a2                	ld	ra,40(sp)
    800019aa:	7402                	ld	s0,32(sp)
    800019ac:	64e2                	ld	s1,24(sp)
    800019ae:	6942                	ld	s2,16(sp)
    800019b0:	69a2                	ld	s3,8(sp)
    800019b2:	6a02                	ld	s4,0(sp)
    800019b4:	6145                	addi	sp,sp,48
    800019b6:	8082                	ret

00000000800019b8 <rfs_w1block>:
int rfs_w1block(struct rfs_device *rfs_dev, int n_block) {
    800019b8:	1141                	addi	sp,sp,-16
    800019ba:	e406                	sd	ra,8(sp)
  return dop_write(rfs_dev, n_block);
    800019bc:	6d1c                	ld	a5,24(a0)
    800019be:	9782                	jalr	a5
}
    800019c0:	60a2                	ld	ra,8(sp)
    800019c2:	0141                	addi	sp,sp,16
    800019c4:	8082                	ret

00000000800019c6 <rfs_read_dinode>:
struct rfs_dinode *rfs_read_dinode(struct rfs_device *rdev, int n_inode) {
    800019c6:	1101                	addi	sp,sp,-32
    800019c8:	ec06                	sd	ra,24(sp)
    800019ca:	e822                	sd	s0,16(sp)
    800019cc:	e426                	sd	s1,8(sp)
    800019ce:	e04a                	sd	s2,0(sp)
    800019d0:	84aa                	mv	s1,a0
  int n_block = n_inode / (RFS_BLKSIZE / RFS_INODESIZE) + RFS_BLK_OFFSET_INODE;
    800019d2:	41f5d71b          	sraiw	a4,a1,0x1f
    800019d6:	01b7571b          	srliw	a4,a4,0x1b
    800019da:	00b707bb          	addw	a5,a4,a1
    800019de:	4057d59b          	sraiw	a1,a5,0x5
  int offset = n_inode % (RFS_BLKSIZE / RFS_INODESIZE);
    800019e2:	8bfd                	andi	a5,a5,31
    800019e4:	40e7843b          	subw	s0,a5,a4
  if (dop_read(rdev, n_block) != 0) return NULL;
    800019e8:	711c                	ld	a5,32(a0)
    800019ea:	2585                	addiw	a1,a1,1
    800019ec:	9782                	jalr	a5
    800019ee:	e11d                	bnez	a0,80001a14 <rfs_read_dinode+0x4e>
  struct rfs_dinode *dinode = (struct rfs_dinode *)alloc_page();
    800019f0:	dedfe0ef          	jal	800007dc <alloc_page>
    800019f4:	892a                	mv	s2,a0
  memcpy(dinode, (char *)rdev->iobuffer + offset * RFS_INODESIZE,
    800019f6:	688c                	ld	a1,16(s1)
    800019f8:	0074141b          	slliw	s0,s0,0x7
    800019fc:	03800613          	li	a2,56
    80001a00:	95a2                	add	a1,a1,s0
    80001a02:	71a020ef          	jal	8000411c <memcpy>
}
    80001a06:	854a                	mv	a0,s2
    80001a08:	60e2                	ld	ra,24(sp)
    80001a0a:	6442                	ld	s0,16(sp)
    80001a0c:	64a2                	ld	s1,8(sp)
    80001a0e:	6902                	ld	s2,0(sp)
    80001a10:	6105                	addi	sp,sp,32
    80001a12:	8082                	ret
  if (dop_read(rdev, n_block) != 0) return NULL;
    80001a14:	4901                	li	s2,0
    80001a16:	bfc5                	j	80001a06 <rfs_read_dinode+0x40>

0000000080001a18 <rfs_disk_stat>:
int rfs_disk_stat(struct vinode *vinode, struct istat *istat) {
    80001a18:	1101                	addi	sp,sp,-32
    80001a1a:	ec06                	sd	ra,24(sp)
    80001a1c:	e822                	sd	s0,16(sp)
    80001a1e:	e426                	sd	s1,8(sp)
    80001a20:	84aa                	mv	s1,a0
    80001a22:	842e                	mv	s0,a1
  struct rfs_device *rdev = rfs_device_list[vinode->sb->s_dev->dev_id];
    80001a24:	653c                	ld	a5,72(a0)
    80001a26:	6f9c                	ld	a5,24(a5)
    80001a28:	5398                	lw	a4,32(a5)
    80001a2a:	070e                	slli	a4,a4,0x3
    80001a2c:	0000b797          	auipc	a5,0xb
    80001a30:	7e478793          	addi	a5,a5,2020 # 8000d210 <rfs_device_list>
    80001a34:	97ba                	add	a5,a5,a4
  struct rfs_dinode *dinode = rfs_read_dinode(rdev, vinode->inum);
    80001a36:	410c                	lw	a1,0(a0)
    80001a38:	6388                	ld	a0,0(a5)
    80001a3a:	f8dff0ef          	jal	800019c6 <rfs_read_dinode>
  if (dinode == NULL) {
    80001a3e:	c50d                	beqz	a0,80001a68 <rfs_disk_stat+0x50>
  istat->st_inum = 1;
    80001a40:	4785                	li	a5,1
    80001a42:	c01c                	sw	a5,0(s0)
  istat->st_inum = vinode->inum;  // get inode number from vinode
    80001a44:	409c                	lw	a5,0(s1)
    80001a46:	c01c                	sw	a5,0(s0)
  istat->st_size = dinode->size;
    80001a48:	411c                	lw	a5,0(a0)
    80001a4a:	c05c                	sw	a5,4(s0)
  istat->st_type = dinode->type;
    80001a4c:	415c                	lw	a5,4(a0)
    80001a4e:	c41c                	sw	a5,8(s0)
  istat->st_nlinks = dinode->nlinks;
    80001a50:	451c                	lw	a5,8(a0)
    80001a52:	c45c                	sw	a5,12(s0)
  istat->st_blocks = dinode->blocks;
    80001a54:	455c                	lw	a5,12(a0)
    80001a56:	c81c                	sw	a5,16(s0)
  free_page(dinode);
    80001a58:	cd5fe0ef          	jal	8000072c <free_page>
  return 0;
    80001a5c:	4501                	li	a0,0
}
    80001a5e:	60e2                	ld	ra,24(sp)
    80001a60:	6442                	ld	s0,16(sp)
    80001a62:	64a2                	ld	s1,8(sp)
    80001a64:	6105                	addi	sp,sp,32
    80001a66:	8082                	ret
    sprint("rfs_disk_stat: read dinode failed!\n");
    80001a68:	00006517          	auipc	a0,0x6
    80001a6c:	1b050513          	addi	a0,a0,432 # 80007c18 <rfs_i_ops+0xba0>
    80001a70:	182030ef          	jal	80004bf2 <sprint>
    return -1;
    80001a74:	557d                	li	a0,-1
    80001a76:	b7e5                	j	80001a5e <rfs_disk_stat+0x46>

0000000080001a78 <rfs_write_dinode>:
                     int n_inode) {
    80001a78:	7179                	addi	sp,sp,-48
    80001a7a:	f406                	sd	ra,40(sp)
    80001a7c:	f022                	sd	s0,32(sp)
    80001a7e:	ec26                	sd	s1,24(sp)
    80001a80:	e84a                	sd	s2,16(sp)
    80001a82:	e44e                	sd	s3,8(sp)
    80001a84:	84aa                	mv	s1,a0
    80001a86:	89ae                	mv	s3,a1
  int n_block = n_inode / (RFS_BLKSIZE / RFS_INODESIZE) + RFS_BLK_OFFSET_INODE;
    80001a88:	41f6579b          	sraiw	a5,a2,0x1f
    80001a8c:	01b7d79b          	srliw	a5,a5,0x1b
    80001a90:	9e3d                	addw	a2,a2,a5
    80001a92:	4056591b          	sraiw	s2,a2,0x5
    80001a96:	2905                	addiw	s2,s2,1
  int offset = n_inode % (RFS_BLKSIZE / RFS_INODESIZE);
    80001a98:	8a7d                	andi	a2,a2,31
    80001a9a:	40f6043b          	subw	s0,a2,a5
  dop_read(rdev, n_block);
    80001a9e:	711c                	ld	a5,32(a0)
    80001aa0:	85ca                	mv	a1,s2
    80001aa2:	9782                	jalr	a5
  memcpy(rdev->iobuffer + offset * RFS_INODESIZE, dinode,
    80001aa4:	6888                	ld	a0,16(s1)
    80001aa6:	0074141b          	slliw	s0,s0,0x7
    80001aaa:	03800613          	li	a2,56
    80001aae:	85ce                	mv	a1,s3
    80001ab0:	9522                	add	a0,a0,s0
    80001ab2:	66a020ef          	jal	8000411c <memcpy>
  int ret = dop_write(rdev, n_block);
    80001ab6:	6c9c                	ld	a5,24(s1)
    80001ab8:	85ca                	mv	a1,s2
    80001aba:	8526                	mv	a0,s1
    80001abc:	9782                	jalr	a5
}
    80001abe:	70a2                	ld	ra,40(sp)
    80001ac0:	7402                	ld	s0,32(sp)
    80001ac2:	64e2                	ld	s1,24(sp)
    80001ac4:	6942                	ld	s2,16(sp)
    80001ac6:	69a2                	ld	s3,8(sp)
    80001ac8:	6145                	addi	sp,sp,48
    80001aca:	8082                	ret

0000000080001acc <rfs_format_dev>:
int rfs_format_dev(struct device *dev) {
    80001acc:	711d                	addi	sp,sp,-96
    80001ace:	ec86                	sd	ra,88(sp)
    80001ad0:	e8a2                	sd	s0,80(sp)
    80001ad2:	e4a6                	sd	s1,72(sp)
    80001ad4:	e0ca                	sd	s2,64(sp)
    80001ad6:	892a                	mv	s2,a0
  struct rfs_device *rdev = rfs_device_list[dev->dev_id];
    80001ad8:	5118                	lw	a4,32(a0)
    80001ada:	070e                	slli	a4,a4,0x3
    80001adc:	0000b797          	auipc	a5,0xb
    80001ae0:	73478793          	addi	a5,a5,1844 # 8000d210 <rfs_device_list>
    80001ae4:	97ba                	add	a5,a5,a4
    80001ae6:	6384                	ld	s1,0(a5)
  struct super_block *super = (struct super_block *)rdev->iobuffer;
    80001ae8:	689c                	ld	a5,16(s1)
  super->magic = RFS_MAGIC;
    80001aea:	6731                	lui	a4,0xc
    80001aec:	eaf70713          	addi	a4,a4,-337 # beaf <elf_fpread-0x7fff4151>
    80001af0:	c398                	sw	a4,0(a5)
  super->size =
    80001af2:	07000713          	li	a4,112
    80001af6:	c3d8                	sw	a4,4(a5)
  super->nblocks = RFS_MAX_INODE_BLKNUM * RFS_DIRECT_BLKNUM;
    80001af8:	06400713          	li	a4,100
    80001afc:	c798                	sw	a4,8(a5)
  super->ninodes = RFS_BLKSIZE / RFS_INODESIZE * RFS_MAX_INODE_BLKNUM;
    80001afe:	14000713          	li	a4,320
    80001b02:	c7d8                	sw	a4,12(a5)
  if (rfs_w1block(rdev, RFS_BLK_OFFSET_SUPER) != 0)  // write to device
    80001b04:	4581                	li	a1,0
    80001b06:	8526                	mv	a0,s1
    80001b08:	eb1ff0ef          	jal	800019b8 <rfs_w1block>
    80001b0c:	e509                	bnez	a0,80001b16 <rfs_format_dev+0x4a>
    80001b0e:	842a                	mv	s0,a0
  struct rfs_dinode *p_dinode = (struct rfs_dinode *)rdev->iobuffer;
    80001b10:	689c                	ld	a5,16(s1)
  for (int i = 0; i < RFS_BLKSIZE / RFS_INODESIZE; ++i) {
    80001b12:	872a                	mv	a4,a0
    80001b14:	a015                	j	80001b38 <rfs_format_dev+0x6c>
    panic("RFS: failed to write superblock!\n");
    80001b16:	00006517          	auipc	a0,0x6
    80001b1a:	12a50513          	addi	a0,a0,298 # 80007c40 <rfs_i_ops+0xbc8>
    80001b1e:	178030ef          	jal	80004c96 <do_panic>
    p_dinode->size = 0;
    80001b22:	0007a023          	sw	zero,0(a5)
    p_dinode->type = R_FREE;
    80001b26:	4689                	li	a3,2
    80001b28:	c3d4                	sw	a3,4(a5)
    p_dinode->nlinks = 0;
    80001b2a:	0007a423          	sw	zero,8(a5)
    p_dinode->blocks = 0;
    80001b2e:	0007a623          	sw	zero,12(a5)
    p_dinode = (struct rfs_dinode *)((char *)p_dinode + RFS_INODESIZE);
    80001b32:	08078793          	addi	a5,a5,128
  for (int i = 0; i < RFS_BLKSIZE / RFS_INODESIZE; ++i) {
    80001b36:	2705                	addiw	a4,a4,1
    80001b38:	46fd                	li	a3,31
    80001b3a:	fee6d4e3          	bge	a3,a4,80001b22 <rfs_format_dev+0x56>
  for (int inode_block = 0; inode_block < RFS_MAX_INODE_BLKNUM; ++inode_block) {
    80001b3e:	47a5                	li	a5,9
    80001b40:	0087ce63          	blt	a5,s0,80001b5c <rfs_format_dev+0x90>
    if (rfs_w1block(rdev, RFS_BLK_OFFSET_INODE + inode_block) != 0)
    80001b44:	2405                	addiw	s0,s0,1
    80001b46:	85a2                	mv	a1,s0
    80001b48:	8526                	mv	a0,s1
    80001b4a:	e6fff0ef          	jal	800019b8 <rfs_w1block>
    80001b4e:	d965                	beqz	a0,80001b3e <rfs_format_dev+0x72>
      panic("RFS: failed to initialize empty inodes!\n");
    80001b50:	00006517          	auipc	a0,0x6
    80001b54:	11850513          	addi	a0,a0,280 # 80007c68 <rfs_i_ops+0xbf0>
    80001b58:	13e030ef          	jal	80004c96 <do_panic>
  root_dinode.size = 0;
    80001b5c:	c402                	sw	zero,8(sp)
  root_dinode.type = R_DIR;
    80001b5e:	4785                	li	a5,1
    80001b60:	c63e                	sw	a5,12(sp)
  root_dinode.nlinks = 1;
    80001b62:	c83e                	sw	a5,16(sp)
  root_dinode.blocks = 1;
    80001b64:	ca3e                	sw	a5,20(sp)
  root_dinode.addrs[0] = RFS_BLK_OFFSET_FREE;
    80001b66:	47b1                	li	a5,12
    80001b68:	cc3e                	sw	a5,24(sp)
  if (rfs_write_dinode(rdev, &root_dinode, 0) != 0) {
    80001b6a:	4601                	li	a2,0
    80001b6c:	002c                	addi	a1,sp,8
    80001b6e:	8526                	mv	a0,s1
    80001b70:	f09ff0ef          	jal	80001a78 <rfs_write_dinode>
    80001b74:	ed0d                	bnez	a0,80001bae <rfs_format_dev+0xe2>
  int *freemap = (int *)rdev->iobuffer;
    80001b76:	6880                	ld	s0,16(s1)
  memset(freemap, 0, RFS_BLKSIZE);
    80001b78:	6605                	lui	a2,0x1
    80001b7a:	4581                	li	a1,0
    80001b7c:	8522                	mv	a0,s0
    80001b7e:	5d6020ef          	jal	80004154 <memset>
  freemap[0] = 1;  // the first data block is used for root directory
    80001b82:	4785                	li	a5,1
    80001b84:	c01c                	sw	a5,0(s0)
  if (rfs_w1block(rdev, RFS_BLK_OFFSET_BITMAP) != 0) {  // write to device
    80001b86:	45ad                	li	a1,11
    80001b88:	8526                	mv	a0,s1
    80001b8a:	e2fff0ef          	jal	800019b8 <rfs_w1block>
    80001b8e:	842a                	mv	s0,a0
    80001b90:	e51d                	bnez	a0,80001bbe <rfs_format_dev+0xf2>
  sprint("RFS: format %s done!\n", dev->dev_name);
    80001b92:	85ca                	mv	a1,s2
    80001b94:	00006517          	auipc	a0,0x6
    80001b98:	14c50513          	addi	a0,a0,332 # 80007ce0 <rfs_i_ops+0xc68>
    80001b9c:	056030ef          	jal	80004bf2 <sprint>
}
    80001ba0:	8522                	mv	a0,s0
    80001ba2:	60e6                	ld	ra,88(sp)
    80001ba4:	6446                	ld	s0,80(sp)
    80001ba6:	64a6                	ld	s1,72(sp)
    80001ba8:	6906                	ld	s2,64(sp)
    80001baa:	6125                	addi	sp,sp,96
    80001bac:	8082                	ret
    sprint("RFS: failed to write root inode!\n");
    80001bae:	00006517          	auipc	a0,0x6
    80001bb2:	0ea50513          	addi	a0,a0,234 # 80007c98 <rfs_i_ops+0xc20>
    80001bb6:	03c030ef          	jal	80004bf2 <sprint>
    return -1;
    80001bba:	547d                	li	s0,-1
    80001bbc:	b7d5                	j	80001ba0 <rfs_format_dev+0xd4>
    sprint("RFS: failed to write bitmap!\n");
    80001bbe:	00006517          	auipc	a0,0x6
    80001bc2:	10250513          	addi	a0,a0,258 # 80007cc0 <rfs_i_ops+0xc48>
    80001bc6:	02c030ef          	jal	80004bf2 <sprint>
    return -1;
    80001bca:	547d                	li	s0,-1
    80001bcc:	bfd1                	j	80001ba0 <rfs_format_dev+0xd4>

0000000080001bce <rfs_write_back_vinode>:
int rfs_write_back_vinode(struct vinode *vinode) {
    80001bce:	715d                	addi	sp,sp,-80
    80001bd0:	e486                	sd	ra,72(sp)
  dinode.size = vinode->size;
    80001bd2:	451c                	lw	a5,8(a0)
    80001bd4:	c43e                	sw	a5,8(sp)
  dinode.nlinks = vinode->nlinks;
    80001bd6:	491c                	lw	a5,16(a0)
    80001bd8:	c83e                	sw	a5,16(sp)
  dinode.blocks = vinode->blocks;
    80001bda:	495c                	lw	a5,20(a0)
    80001bdc:	ca3e                	sw	a5,20(sp)
  dinode.type = vinode->type;
    80001bde:	455c                	lw	a5,12(a0)
    80001be0:	c63e                	sw	a5,12(sp)
  for (int i = 0; i < RFS_DIRECT_BLKNUM; ++i) {
    80001be2:	4701                	li	a4,0
    80001be4:	a829                	j	80001bfe <rfs_write_back_vinode+0x30>
    dinode.addrs[i] = vinode->addrs[i];
    80001be6:	00470793          	addi	a5,a4,4
    80001bea:	078a                	slli	a5,a5,0x2
    80001bec:	00f506b3          	add	a3,a0,a5
    80001bf0:	4694                	lw	a3,8(a3)
    80001bf2:	04078793          	addi	a5,a5,64
    80001bf6:	978a                	add	a5,a5,sp
    80001bf8:	fcd7a423          	sw	a3,-56(a5)
  for (int i = 0; i < RFS_DIRECT_BLKNUM; ++i) {
    80001bfc:	2705                	addiw	a4,a4,1
    80001bfe:	47a5                	li	a5,9
    80001c00:	fee7d3e3          	bge	a5,a4,80001be6 <rfs_write_back_vinode+0x18>
  struct rfs_device *rdev = rfs_device_list[vinode->sb->s_dev->dev_id];
    80001c04:	653c                	ld	a5,72(a0)
    80001c06:	6f9c                	ld	a5,24(a5)
    80001c08:	5398                	lw	a4,32(a5)
    80001c0a:	070e                	slli	a4,a4,0x3
    80001c0c:	0000b797          	auipc	a5,0xb
    80001c10:	60478793          	addi	a5,a5,1540 # 8000d210 <rfs_device_list>
    80001c14:	97ba                	add	a5,a5,a4
  if (rfs_write_dinode(rdev, &dinode, vinode->inum) != 0) {
    80001c16:	4110                	lw	a2,0(a0)
    80001c18:	002c                	addi	a1,sp,8
    80001c1a:	6388                	ld	a0,0(a5)
    80001c1c:	e5dff0ef          	jal	80001a78 <rfs_write_dinode>
    80001c20:	e501                	bnez	a0,80001c28 <rfs_write_back_vinode+0x5a>
}
    80001c22:	60a6                	ld	ra,72(sp)
    80001c24:	6161                	addi	sp,sp,80
    80001c26:	8082                	ret
    sprint("rfs_free_write_back_inode: failed to write back disk inode!\n");
    80001c28:	00006517          	auipc	a0,0x6
    80001c2c:	0d050513          	addi	a0,a0,208 # 80007cf8 <rfs_i_ops+0xc80>
    80001c30:	7c3020ef          	jal	80004bf2 <sprint>
    return -1;
    80001c34:	557d                	li	a0,-1
    80001c36:	b7f5                	j	80001c22 <rfs_write_back_vinode+0x54>

0000000080001c38 <rfs_alloc_block>:
  int *freemap = (int *)sb->s_fs_info;
    80001c38:	7110                	ld	a2,32(a0)
  for (int block = 0; block < sb->nblocks; ++block) {
    80001c3a:	4781                	li	a5,0
    80001c3c:	a011                	j	80001c40 <rfs_alloc_block+0x8>
    80001c3e:	2785                	addiw	a5,a5,1
    80001c40:	4518                	lw	a4,8(a0)
    80001c42:	00e7dc63          	bge	a5,a4,80001c5a <rfs_alloc_block+0x22>
    if (freemap[block] == 0) {  // find a free block
    80001c46:	00279713          	slli	a4,a5,0x2
    80001c4a:	9732                	add	a4,a4,a2
    80001c4c:	4314                	lw	a3,0(a4)
    80001c4e:	fae5                	bnez	a3,80001c3e <rfs_alloc_block+0x6>
      freemap[block] = 1;
    80001c50:	4685                	li	a3,1
    80001c52:	c314                	sw	a3,0(a4)
      free_block = RFS_BLK_OFFSET_FREE + block;
    80001c54:	00c7851b          	addiw	a0,a5,12
      break;
    80001c58:	a011                	j	80001c5c <rfs_alloc_block+0x24>
  int free_block = -1;
    80001c5a:	557d                	li	a0,-1
  if (free_block == -1) panic("rfs_alloc_block: no more free block!\n");
    80001c5c:	57fd                	li	a5,-1
    80001c5e:	00f50363          	beq	a0,a5,80001c64 <rfs_alloc_block+0x2c>
}
    80001c62:	8082                	ret
int rfs_alloc_block(struct super_block *sb) {
    80001c64:	1141                	addi	sp,sp,-16
    80001c66:	e406                	sd	ra,8(sp)
  if (free_block == -1) panic("rfs_alloc_block: no more free block!\n");
    80001c68:	00006517          	auipc	a0,0x6
    80001c6c:	0d050513          	addi	a0,a0,208 # 80007d38 <rfs_i_ops+0xcc0>
    80001c70:	026030ef          	jal	80004c96 <do_panic>

0000000080001c74 <rfs_write>:
                  int *offset) {
    80001c74:	7159                	addi	sp,sp,-112
    80001c76:	f486                	sd	ra,104(sp)
    80001c78:	f0a2                	sd	s0,96(sp)
    80001c7a:	eca6                	sd	s1,88(sp)
    80001c7c:	e8ca                	sd	s2,80(sp)
    80001c7e:	e4ce                	sd	s3,72(sp)
    80001c80:	e0d2                	sd	s4,64(sp)
    80001c82:	fc56                	sd	s5,56(sp)
    80001c84:	f85a                	sd	s6,48(sp)
    80001c86:	f45e                	sd	s7,40(sp)
    80001c88:	f062                	sd	s8,32(sp)
    80001c8a:	ec66                	sd	s9,24(sp)
    80001c8c:	e86a                	sd	s10,16(sp)
    80001c8e:	e46e                	sd	s11,8(sp)
  if (f_inode->size < *offset) {
    80001c90:	451c                	lw	a5,8(a0)
    80001c92:	0006ab03          	lw	s6,0(a3)
    80001c96:	0b67c863          	blt	a5,s6,80001d46 <rfs_write+0xd2>
    80001c9a:	892a                	mv	s2,a0
    80001c9c:	8c2e                	mv	s8,a1
    80001c9e:	8a32                	mv	s4,a2
    80001ca0:	8ab6                	mv	s5,a3
  int align = *offset % RFS_BLKSIZE;
    80001ca2:	41fb571b          	sraiw	a4,s6,0x1f
    80001ca6:	0147571b          	srliw	a4,a4,0x14
    80001caa:	01670dbb          	addw	s11,a4,s6
    80001cae:	6785                	lui	a5,0x1
    80001cb0:	17fd                	addi	a5,a5,-1 # fff <elf_fpread-0x7ffff001>
    80001cb2:	00fdfcb3          	and	s9,s11,a5
    80001cb6:	40ec8cbb          	subw	s9,s9,a4
    80001cba:	000c8d1b          	sext.w	s10,s9
  int writetimes = (len + align) / RFS_BLKSIZE;
    80001cbe:	00cd0bb3          	add	s7,s10,a2
    80001cc2:	43fbd713          	srai	a4,s7,0x3f
    80001cc6:	00f774b3          	and	s1,a4,a5
    80001cca:	94de                	add	s1,s1,s7
    80001ccc:	84b1                	srai	s1,s1,0xc
    80001cce:	2481                	sext.w	s1,s1
  int remain = (len + align) % RFS_BLKSIZE;
    80001cd0:	9351                	srli	a4,a4,0x34
    80001cd2:	9bba                	add	s7,s7,a4
    80001cd4:	00fbfbb3          	and	s7,s7,a5
    80001cd8:	40eb8bbb          	subw	s7,s7,a4
  int block_offset = *offset / RFS_BLKSIZE;
    80001cdc:	40cddd9b          	sraiw	s11,s11,0xc
    80001ce0:	000d841b          	sext.w	s0,s11
  struct rfs_device *rdev = rfs_device_list[f_inode->sb->s_dev->dev_id];
    80001ce4:	6538                	ld	a4,72(a0)
    80001ce6:	6f18                	ld	a4,24(a4)
    80001ce8:	5314                	lw	a3,32(a4)
    80001cea:	068e                	slli	a3,a3,0x3
    80001cec:	0000b717          	auipc	a4,0xb
    80001cf0:	52470713          	addi	a4,a4,1316 # 8000d210 <rfs_device_list>
    80001cf4:	9736                	add	a4,a4,a3
    80001cf6:	00073983          	ld	s3,0(a4)
  if (align != 0) {
    80001cfa:	00fb7b33          	and	s6,s6,a5
    80001cfe:	040b1a63          	bnez	s6,80001d52 <rfs_write+0xde>
  if (writetimes >= 0) {
    80001d02:	0a04de63          	bgez	s1,80001dbe <rfs_write+0x14a>
      (f_inode->size < *offset + len ? *offset + len : f_inode->size);
    80001d06:	000aa783          	lw	a5,0(s5)
    80001d0a:	97d2                	add	a5,a5,s4
    80001d0c:	00892703          	lw	a4,8(s2)
    80001d10:	00f75363          	bge	a4,a5,80001d16 <rfs_write+0xa2>
    80001d14:	873e                	mv	a4,a5
  f_inode->size =
    80001d16:	00e92423          	sw	a4,8(s2)
  *offset += len;
    80001d1a:	000aa783          	lw	a5,0(s5)
    80001d1e:	00fa07bb          	addw	a5,s4,a5
    80001d22:	00faa023          	sw	a5,0(s5)
}
    80001d26:	8552                	mv	a0,s4
    80001d28:	70a6                	ld	ra,104(sp)
    80001d2a:	7406                	ld	s0,96(sp)
    80001d2c:	64e6                	ld	s1,88(sp)
    80001d2e:	6946                	ld	s2,80(sp)
    80001d30:	69a6                	ld	s3,72(sp)
    80001d32:	6a06                	ld	s4,64(sp)
    80001d34:	7ae2                	ld	s5,56(sp)
    80001d36:	7b42                	ld	s6,48(sp)
    80001d38:	7ba2                	ld	s7,40(sp)
    80001d3a:	7c02                	ld	s8,32(sp)
    80001d3c:	6ce2                	ld	s9,24(sp)
    80001d3e:	6d42                	ld	s10,16(sp)
    80001d40:	6da2                	ld	s11,8(sp)
    80001d42:	6165                	addi	sp,sp,112
    80001d44:	8082                	ret
    panic("rfs_write:offset should less than file size!");
    80001d46:	00006517          	auipc	a0,0x6
    80001d4a:	01a50513          	addi	a0,a0,26 # 80007d60 <rfs_i_ops+0xce8>
    80001d4e:	749020ef          	jal	80004c96 <do_panic>
    rfs_r1block(rdev, f_inode->addrs[block_offset]);
    80001d52:	00440793          	addi	a5,s0,4
    80001d56:	078a                	slli	a5,a5,0x2
    80001d58:	97aa                	add	a5,a5,a0
    80001d5a:	478c                	lw	a1,8(a5)
    80001d5c:	854e                	mv	a0,s3
    80001d5e:	a4bff0ef          	jal	800017a8 <rfs_r1block>
    int first_block_len = (writetimes == 0 ? len : RFS_BLKSIZE - align);
    80001d62:	c49d                	beqz	s1,80001d90 <rfs_write+0x11c>
    80001d64:	6b05                	lui	s6,0x1
    80001d66:	419b0b3b          	subw	s6,s6,s9
    memcpy(rdev->iobuffer + align, w_buf, first_block_len);
    80001d6a:	0109b503          	ld	a0,16(s3)
    80001d6e:	865a                	mv	a2,s6
    80001d70:	85e2                	mv	a1,s8
    80001d72:	956a                	add	a0,a0,s10
    80001d74:	3a8020ef          	jal	8000411c <memcpy>
    rfs_w1block(rdev, f_inode->addrs[block_offset]);
    80001d78:	00440793          	addi	a5,s0,4
    80001d7c:	078a                	slli	a5,a5,0x2
    80001d7e:	97ca                	add	a5,a5,s2
    80001d80:	478c                	lw	a1,8(a5)
    80001d82:	854e                	mv	a0,s3
    80001d84:	c35ff0ef          	jal	800019b8 <rfs_w1block>
    block_offset++;
    80001d88:	001d841b          	addiw	s0,s11,1
    writetimes--;
    80001d8c:	34fd                	addiw	s1,s1,-1
    80001d8e:	bf95                	j	80001d02 <rfs_write+0x8e>
    int first_block_len = (writetimes == 0 ? len : RFS_BLKSIZE - align);
    80001d90:	000a0b1b          	sext.w	s6,s4
    80001d94:	bfd9                	j	80001d6a <rfs_write+0xf6>
      memcpy(rdev->iobuffer, w_buf + buf_offset, RFS_BLKSIZE);
    80001d96:	6605                	lui	a2,0x1
    80001d98:	016c05b3          	add	a1,s8,s6
    80001d9c:	0109b503          	ld	a0,16(s3)
    80001da0:	37c020ef          	jal	8000411c <memcpy>
      rfs_w1block(rdev, f_inode->addrs[block_offset]);
    80001da4:	00440793          	addi	a5,s0,4
    80001da8:	078a                	slli	a5,a5,0x2
    80001daa:	97ca                	add	a5,a5,s2
    80001dac:	478c                	lw	a1,8(a5)
    80001dae:	854e                	mv	a0,s3
    80001db0:	c09ff0ef          	jal	800019b8 <rfs_w1block>
      buf_offset += RFS_BLKSIZE;
    80001db4:	6785                	lui	a5,0x1
    80001db6:	01678b3b          	addw	s6,a5,s6
      block_offset++;
    80001dba:	2405                	addiw	s0,s0,1
      writetimes--;
    80001dbc:	34fd                	addiw	s1,s1,-1
    while (writetimes != 0) {
    80001dbe:	c485                	beqz	s1,80001de6 <rfs_write+0x172>
      if (block_offset == f_inode->blocks) {  // need to create new block
    80001dc0:	01492783          	lw	a5,20(s2)
    80001dc4:	fc8799e3          	bne	a5,s0,80001d96 <rfs_write+0x122>
        f_inode->addrs[block_offset] = rfs_alloc_block(f_inode->sb);
    80001dc8:	04893503          	ld	a0,72(s2)
    80001dcc:	e6dff0ef          	jal	80001c38 <rfs_alloc_block>
    80001dd0:	00440793          	addi	a5,s0,4
    80001dd4:	078a                	slli	a5,a5,0x2
    80001dd6:	97ca                	add	a5,a5,s2
    80001dd8:	c788                	sw	a0,8(a5)
        f_inode->blocks++;
    80001dda:	01492783          	lw	a5,20(s2)
    80001dde:	2785                	addiw	a5,a5,1 # 1001 <elf_fpread-0x7fffefff>
    80001de0:	00f92a23          	sw	a5,20(s2)
    80001de4:	bf4d                	j	80001d96 <rfs_write+0x122>
    if (remain > 0) {
    80001de6:	f37050e3          	blez	s7,80001d06 <rfs_write+0x92>
      if (block_offset == f_inode->blocks) {
    80001dea:	01492783          	lw	a5,20(s2)
    80001dee:	02878163          	beq	a5,s0,80001e10 <rfs_write+0x19c>
      memcpy(rdev->iobuffer, w_buf + buf_offset, remain);
    80001df2:	865e                	mv	a2,s7
    80001df4:	016c05b3          	add	a1,s8,s6
    80001df8:	0109b503          	ld	a0,16(s3)
    80001dfc:	320020ef          	jal	8000411c <memcpy>
      rfs_w1block(rdev, f_inode->addrs[block_offset]);
    80001e00:	0411                	addi	s0,s0,4
    80001e02:	040a                	slli	s0,s0,0x2
    80001e04:	944a                	add	s0,s0,s2
    80001e06:	440c                	lw	a1,8(s0)
    80001e08:	854e                	mv	a0,s3
    80001e0a:	bafff0ef          	jal	800019b8 <rfs_w1block>
    80001e0e:	bde5                	j	80001d06 <rfs_write+0x92>
        f_inode->addrs[block_offset] = rfs_alloc_block(f_inode->sb);
    80001e10:	04893503          	ld	a0,72(s2)
    80001e14:	e25ff0ef          	jal	80001c38 <rfs_alloc_block>
    80001e18:	00440793          	addi	a5,s0,4
    80001e1c:	078a                	slli	a5,a5,0x2
    80001e1e:	97ca                	add	a5,a5,s2
    80001e20:	c788                	sw	a0,8(a5)
        ++f_inode->blocks;
    80001e22:	01492783          	lw	a5,20(s2)
    80001e26:	2785                	addiw	a5,a5,1
    80001e28:	00f92a23          	sw	a5,20(s2)
    80001e2c:	b7d9                	j	80001df2 <rfs_write+0x17e>

0000000080001e2e <rfs_free_block>:
  int *freemap = (int *)sb->s_fs_info;
    80001e2e:	711c                	ld	a5,32(a0)
  freemap[block_num - RFS_BLK_OFFSET_FREE] = 0;
    80001e30:	058a                	slli	a1,a1,0x2
    80001e32:	fd058593          	addi	a1,a1,-48
    80001e36:	97ae                	add	a5,a5,a1
    80001e38:	0007a023          	sw	zero,0(a5)
}
    80001e3c:	4501                	li	a0,0
    80001e3e:	8082                	ret

0000000080001e40 <rfs_unlink>:
int rfs_unlink(struct vinode *parent, struct dentry *sub_dentry, struct vinode *unlink_vinode) {
    80001e40:	715d                	addi	sp,sp,-80
    80001e42:	e486                	sd	ra,72(sp)
    80001e44:	e0a2                	sd	s0,64(sp)
    80001e46:	fc26                	sd	s1,56(sp)
    80001e48:	f84a                	sd	s2,48(sp)
    80001e4a:	f44e                	sd	s3,40(sp)
    80001e4c:	f052                	sd	s4,32(sp)
    80001e4e:	ec56                	sd	s5,24(sp)
    80001e50:	e85a                	sd	s6,16(sp)
    80001e52:	e45e                	sd	s7,8(sp)
    80001e54:	892a                	mv	s2,a0
    80001e56:	8a2e                	mv	s4,a1
    80001e58:	8ab2                	mv	s5,a2
  struct rfs_device *rdev = rfs_device_list[parent->sb->s_dev->dev_id];
    80001e5a:	653c                	ld	a5,72(a0)
    80001e5c:	6f9c                	ld	a5,24(a5)
    80001e5e:	5398                	lw	a4,32(a5)
    80001e60:	070e                	slli	a4,a4,0x3
    80001e62:	0000b797          	auipc	a5,0xb
    80001e66:	3ae78793          	addi	a5,a5,942 # 8000d210 <rfs_device_list>
    80001e6a:	97ba                	add	a5,a5,a4
    80001e6c:	6380                	ld	s0,0(a5)
  int total_direntrys = parent->size / sizeof(struct rfs_direntry);
    80001e6e:	00852983          	lw	s3,8(a0)
    80001e72:	4059d993          	srai	s3,s3,0x5
  for (delete_index = 0; delete_index < total_direntrys; ++delete_index) {
    80001e76:	4481                	li	s1,0
  struct rfs_direntry *p_direntry = NULL;
    80001e78:	4b01                	li	s6,0
  for (delete_index = 0; delete_index < total_direntrys; ++delete_index) {
    80001e7a:	a811                	j	80001e8e <rfs_unlink+0x4e>
    if (strcmp(p_direntry->name, sub_dentry->name) == 0) {  // found
    80001e7c:	85d2                	mv	a1,s4
    80001e7e:	004b0513          	addi	a0,s6,4 # 1004 <elf_fpread-0x7fffeffc>
    80001e82:	328020ef          	jal	800041aa <strcmp>
    80001e86:	c915                	beqz	a0,80001eba <rfs_unlink+0x7a>
    ++p_direntry;
    80001e88:	020b0b13          	addi	s6,s6,32
  for (delete_index = 0; delete_index < total_direntrys; ++delete_index) {
    80001e8c:	2485                	addiw	s1,s1,1
    80001e8e:	0334d663          	bge	s1,s3,80001eba <rfs_unlink+0x7a>
    if (delete_index % one_block_direntrys == 0) {
    80001e92:	07f4f793          	andi	a5,s1,127
    80001e96:	f3fd                	bnez	a5,80001e7c <rfs_unlink+0x3c>
      rfs_r1block(rdev, parent->addrs[delete_index / one_block_direntrys]);
    80001e98:	41f4d79b          	sraiw	a5,s1,0x1f
    80001e9c:	0197d79b          	srliw	a5,a5,0x19
    80001ea0:	9fa5                	addw	a5,a5,s1
    80001ea2:	4077d79b          	sraiw	a5,a5,0x7
    80001ea6:	0791                	addi	a5,a5,4
    80001ea8:	078a                	slli	a5,a5,0x2
    80001eaa:	97ca                	add	a5,a5,s2
    80001eac:	478c                	lw	a1,8(a5)
    80001eae:	8522                	mv	a0,s0
    80001eb0:	8f9ff0ef          	jal	800017a8 <rfs_r1block>
      p_direntry = (struct rfs_direntry *)rdev->iobuffer;
    80001eb4:	01043b03          	ld	s6,16(s0)
    80001eb8:	b7d1                	j	80001e7c <rfs_unlink+0x3c>
  int inum = p_direntry->inum;
    80001eba:	000b2b83          	lw	s7,0(s6)
  if (delete_index == total_direntrys) {
    80001ebe:	09348963          	beq	s1,s3,80001f50 <rfs_unlink+0x110>
  struct rfs_dinode *unlink_dinode = rfs_read_dinode(rdev, inum);
    80001ec2:	85de                	mv	a1,s7
    80001ec4:	8522                	mv	a0,s0
    80001ec6:	b01ff0ef          	jal	800019c6 <rfs_read_dinode>
    80001eca:	8b2a                	mv	s6,a0
  assert(unlink_vinode->nlinks == unlink_dinode->nlinks);
    80001ecc:	010aa783          	lw	a5,16(s5)
    80001ed0:	4518                	lw	a4,8(a0)
    80001ed2:	08e79863          	bne	a5,a4,80001f62 <rfs_unlink+0x122>
  unlink_vinode->nlinks--;
    80001ed6:	37fd                	addiw	a5,a5,-1
    80001ed8:	00078a1b          	sext.w	s4,a5
    80001edc:	00faa823          	sw	a5,16(s5)
  unlink_dinode->nlinks = unlink_vinode->nlinks;
    80001ee0:	c51c                	sw	a5,8(a0)
  if (unlink_dinode->nlinks == 0) {
    80001ee2:	0a0a0e63          	beqz	s4,80001f9e <rfs_unlink+0x15e>
  rfs_write_dinode(rdev, unlink_dinode, inum);
    80001ee6:	865e                	mv	a2,s7
    80001ee8:	85da                	mv	a1,s6
    80001eea:	8522                	mv	a0,s0
    80001eec:	b8dff0ef          	jal	80001a78 <rfs_write_dinode>
  free_page(unlink_dinode);
    80001ef0:	855a                	mv	a0,s6
    80001ef2:	83bfe0ef          	jal	8000072c <free_page>
  int delete_block_index = delete_index / one_block_direntrys;
    80001ef6:	41f4da9b          	sraiw	s5,s1,0x1f
    80001efa:	019ada9b          	srliw	s5,s5,0x19
    80001efe:	009a84bb          	addw	s1,s5,s1
    80001f02:	4074da1b          	sraiw	s4,s1,0x7
    80001f06:	000a079b          	sext.w	a5,s4
  rfs_r1block(rdev, parent->addrs[delete_block_index]);
    80001f0a:	0791                	addi	a5,a5,4
    80001f0c:	078a                	slli	a5,a5,0x2
    80001f0e:	97ca                	add	a5,a5,s2
    80001f10:	478c                	lw	a1,8(a5)
    80001f12:	8522                	mv	a0,s0
    80001f14:	895ff0ef          	jal	800017a8 <rfs_r1block>
  int offset = delete_index % one_block_direntrys;
    80001f18:	07f4f493          	andi	s1,s1,127
    80001f1c:	415484bb          	subw	s1,s1,s5
    80001f20:	0004879b          	sext.w	a5,s1
  memmove(rdev->iobuffer + offset * sizeof(struct rfs_direntry),
    80001f24:	6808                	ld	a0,16(s0)
    80001f26:	0796                	slli	a5,a5,0x5
          rdev->iobuffer + (offset + 1) * sizeof(struct rfs_direntry),
    80001f28:	0014859b          	addiw	a1,s1,1
    80001f2c:	0596                	slli	a1,a1,0x5
          (one_block_direntrys - offset - 1) * sizeof(struct rfs_direntry));
    80001f2e:	07f00613          	li	a2,127
    80001f32:	9e05                	subw	a2,a2,s1
  memmove(rdev->iobuffer + offset * sizeof(struct rfs_direntry),
    80001f34:	0616                	slli	a2,a2,0x5
    80001f36:	95aa                	add	a1,a1,a0
    80001f38:	953e                	add	a0,a0,a5
    80001f3a:	3b6020ef          	jal	800042f0 <memmove>
  struct rfs_direntry *previous_block = (struct rfs_direntry *)alloc_page();
    80001f3e:	89ffe0ef          	jal	800007dc <alloc_page>
    80001f42:	8aaa                	mv	s5,a0
  memcpy(previous_block, rdev->iobuffer, RFS_BLKSIZE);
    80001f44:	6605                	lui	a2,0x1
    80001f46:	680c                	ld	a1,16(s0)
    80001f48:	1d4020ef          	jal	8000411c <memcpy>
  for (int i = delete_block_index + 1; i < parent->blocks; i++) {
    80001f4c:	2a05                	addiw	s4,s4,1
    80001f4e:	a0c9                	j	80002010 <rfs_unlink+0x1d0>
    sprint("unlink: file %s not found.\n", sub_dentry->name);
    80001f50:	85d2                	mv	a1,s4
    80001f52:	00006517          	auipc	a0,0x6
    80001f56:	e3e50513          	addi	a0,a0,-450 # 80007d90 <rfs_i_ops+0xd18>
    80001f5a:	499020ef          	jal	80004bf2 <sprint>
    return -1;
    80001f5e:	557d                	li	a0,-1
    80001f60:	a209                	j	80002062 <rfs_unlink+0x222>
  assert(unlink_vinode->nlinks == unlink_dinode->nlinks);
    80001f62:	00006697          	auipc	a3,0x6
    80001f66:	e4e68693          	addi	a3,a3,-434 # 80007db0 <rfs_i_ops+0xd38>
    80001f6a:	27f00613          	li	a2,639
    80001f6e:	00006597          	auipc	a1,0x6
    80001f72:	e7258593          	addi	a1,a1,-398 # 80007de0 <rfs_i_ops+0xd68>
    80001f76:	00006517          	auipc	a0,0x6
    80001f7a:	81250513          	addi	a0,a0,-2030 # 80007788 <rfs_i_ops+0x710>
    80001f7e:	475020ef          	jal	80004bf2 <sprint>
    80001f82:	6541                	lui	a0,0x10
    80001f84:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    80001f86:	48d020ef          	jal	80004c12 <poweroff>
      rfs_free_block(parent->sb, unlink_dinode->addrs[i]);
    80001f8a:	004a0793          	addi	a5,s4,4
    80001f8e:	078a                	slli	a5,a5,0x2
    80001f90:	97da                	add	a5,a5,s6
    80001f92:	438c                	lw	a1,0(a5)
    80001f94:	04893503          	ld	a0,72(s2)
    80001f98:	e97ff0ef          	jal	80001e2e <rfs_free_block>
    for (int i = 0; i < unlink_dinode->blocks; ++i) {
    80001f9c:	2a05                	addiw	s4,s4,1
    80001f9e:	00cb2783          	lw	a5,12(s6)
    80001fa2:	fefa44e3          	blt	s4,a5,80001f8a <rfs_unlink+0x14a>
    unlink_dinode->type = R_FREE;
    80001fa6:	4789                	li	a5,2
    80001fa8:	00fb2223          	sw	a5,4(s6)
    80001fac:	bf2d                	j	80001ee6 <rfs_unlink+0xa6>
    rfs_r1block(rdev, parent->addrs[i]);
    80001fae:	004a0793          	addi	a5,s4,4
    80001fb2:	078a                	slli	a5,a5,0x2
    80001fb4:	97ca                	add	a5,a5,s2
    80001fb6:	478c                	lw	a1,8(a5)
    80001fb8:	8522                	mv	a0,s0
    80001fba:	feeff0ef          	jal	800017a8 <rfs_r1block>
    struct rfs_direntry *this_block = (struct rfs_direntry *)alloc_page();
    80001fbe:	81ffe0ef          	jal	800007dc <alloc_page>
    80001fc2:	84aa                	mv	s1,a0
    memcpy(this_block, rdev->iobuffer, RFS_BLKSIZE);
    80001fc4:	6605                	lui	a2,0x1
    80001fc6:	680c                	ld	a1,16(s0)
    80001fc8:	154020ef          	jal	8000411c <memcpy>
    memcpy(previous_block + one_block_direntrys - 1, rdev->iobuffer,
    80001fcc:	6b05                	lui	s6,0x1
    80001fce:	1b01                	addi	s6,s6,-32 # fe0 <elf_fpread-0x7ffff020>
    80001fd0:	02000613          	li	a2,32
    80001fd4:	680c                	ld	a1,16(s0)
    80001fd6:	016a8533          	add	a0,s5,s6
    80001fda:	142020ef          	jal	8000411c <memcpy>
    memmove(this_block, this_block + 1,
    80001fde:	865a                	mv	a2,s6
    80001fe0:	02048593          	addi	a1,s1,32
    80001fe4:	8526                	mv	a0,s1
    80001fe6:	30a020ef          	jal	800042f0 <memmove>
    memcpy(rdev->iobuffer, previous_block, RFS_BLKSIZE);
    80001fea:	6605                	lui	a2,0x1
    80001fec:	85d6                	mv	a1,s5
    80001fee:	6808                	ld	a0,16(s0)
    80001ff0:	12c020ef          	jal	8000411c <memcpy>
    rfs_w1block(rdev, parent->addrs[i - 1]);
    80001ff4:	fffa079b          	addiw	a5,s4,-1
    80001ff8:	0791                	addi	a5,a5,4
    80001ffa:	078a                	slli	a5,a5,0x2
    80001ffc:	97ca                	add	a5,a5,s2
    80001ffe:	478c                	lw	a1,8(a5)
    80002000:	8522                	mv	a0,s0
    80002002:	9b7ff0ef          	jal	800019b8 <rfs_w1block>
    free_page(previous_block);
    80002006:	8556                	mv	a0,s5
    80002008:	f24fe0ef          	jal	8000072c <free_page>
  for (int i = delete_block_index + 1; i < parent->blocks; i++) {
    8000200c:	2a05                	addiw	s4,s4,1
    previous_block = this_block;
    8000200e:	8aa6                	mv	s5,s1
  for (int i = delete_block_index + 1; i < parent->blocks; i++) {
    80002010:	01492783          	lw	a5,20(s2)
    80002014:	f8fa4de3          	blt	s4,a5,80001fae <rfs_unlink+0x16e>
  memcpy(rdev->iobuffer, previous_block, RFS_BLKSIZE);
    80002018:	6605                	lui	a2,0x1
    8000201a:	85d6                	mv	a1,s5
    8000201c:	6808                	ld	a0,16(s0)
    8000201e:	0fe020ef          	jal	8000411c <memcpy>
  rfs_w1block(rdev, parent->addrs[parent->blocks - 1]);
    80002022:	01492783          	lw	a5,20(s2)
    80002026:	37fd                	addiw	a5,a5,-1
    80002028:	0791                	addi	a5,a5,4
    8000202a:	078a                	slli	a5,a5,0x2
    8000202c:	97ca                	add	a5,a5,s2
    8000202e:	478c                	lw	a1,8(a5)
    80002030:	8522                	mv	a0,s0
    80002032:	987ff0ef          	jal	800019b8 <rfs_w1block>
  free_page(previous_block);
    80002036:	8556                	mv	a0,s5
    80002038:	ef4fe0ef          	jal	8000072c <free_page>
  total_direntrys--;
    8000203c:	39fd                	addiw	s3,s3,-1
  if (total_direntrys % one_block_direntrys == 0 && parent->blocks > 1) {
    8000203e:	07f9f993          	andi	s3,s3,127
    80002042:	00099763          	bnez	s3,80002050 <rfs_unlink+0x210>
    80002046:	01492783          	lw	a5,20(s2)
    8000204a:	4705                	li	a4,1
    8000204c:	02f74663          	blt	a4,a5,80002078 <rfs_unlink+0x238>
  parent->size -= sizeof(struct rfs_direntry);
    80002050:	00892783          	lw	a5,8(s2)
    80002054:	3781                	addiw	a5,a5,-32
    80002056:	00f92423          	sw	a5,8(s2)
  if (rfs_write_back_vinode(parent) != 0) {
    8000205a:	854a                	mv	a0,s2
    8000205c:	b73ff0ef          	jal	80001bce <rfs_write_back_vinode>
    80002060:	e91d                	bnez	a0,80002096 <rfs_unlink+0x256>
}
    80002062:	60a6                	ld	ra,72(sp)
    80002064:	6406                	ld	s0,64(sp)
    80002066:	74e2                	ld	s1,56(sp)
    80002068:	7942                	ld	s2,48(sp)
    8000206a:	79a2                	ld	s3,40(sp)
    8000206c:	7a02                	ld	s4,32(sp)
    8000206e:	6ae2                	ld	s5,24(sp)
    80002070:	6b42                	ld	s6,16(sp)
    80002072:	6ba2                	ld	s7,8(sp)
    80002074:	6161                	addi	sp,sp,80
    80002076:	8082                	ret
    rfs_free_block(parent->sb, parent->addrs[parent->blocks - 1]);
    80002078:	37fd                	addiw	a5,a5,-1
    8000207a:	0791                	addi	a5,a5,4
    8000207c:	078a                	slli	a5,a5,0x2
    8000207e:	97ca                	add	a5,a5,s2
    80002080:	478c                	lw	a1,8(a5)
    80002082:	04893503          	ld	a0,72(s2)
    80002086:	da9ff0ef          	jal	80001e2e <rfs_free_block>
    parent->blocks--;
    8000208a:	01492783          	lw	a5,20(s2)
    8000208e:	37fd                	addiw	a5,a5,-1
    80002090:	00f92a23          	sw	a5,20(s2)
    80002094:	bf75                	j	80002050 <rfs_unlink+0x210>
    sprint("rfs_unlink: rfs_write_back_vinode failed");
    80002096:	00006517          	auipc	a0,0x6
    8000209a:	d5a50513          	addi	a0,a0,-678 # 80007df0 <rfs_i_ops+0xd78>
    8000209e:	355020ef          	jal	80004bf2 <sprint>
    return -1;
    800020a2:	557d                	li	a0,-1
    800020a4:	bf7d                	j	80002062 <rfs_unlink+0x222>

00000000800020a6 <rfs_add_direntry>:
int rfs_add_direntry(struct vinode *dir, const char *name, int inum) {
    800020a6:	7179                	addi	sp,sp,-48
    800020a8:	f406                	sd	ra,40(sp)
  if (dir->type != DIR_I) {
    800020aa:	4558                	lw	a4,12(a0)
    800020ac:	4785                	li	a5,1
    800020ae:	08f71f63          	bne	a4,a5,8000214c <rfs_add_direntry+0xa6>
    800020b2:	f022                	sd	s0,32(sp)
    800020b4:	ec26                	sd	s1,24(sp)
    800020b6:	e84a                	sd	s2,16(sp)
    800020b8:	e44e                	sd	s3,8(sp)
    800020ba:	e052                	sd	s4,0(sp)
    800020bc:	842a                	mv	s0,a0
    800020be:	84ae                	mv	s1,a1
    800020c0:	8932                	mv	s2,a2
  struct rfs_device *rdev = rfs_device_list[dir->sb->s_dev->dev_id];
    800020c2:	653c                	ld	a5,72(a0)
    800020c4:	6f9c                	ld	a5,24(a5)
    800020c6:	5398                	lw	a4,32(a5)
    800020c8:	070e                	slli	a4,a4,0x3
    800020ca:	0000b797          	auipc	a5,0xb
    800020ce:	14678793          	addi	a5,a5,326 # 8000d210 <rfs_device_list>
    800020d2:	97ba                	add	a5,a5,a4
    800020d4:	0007b983          	ld	s3,0(a5)
  int n_block = dir->addrs[dir->size / RFS_BLKSIZE];
    800020d8:	4518                	lw	a4,8(a0)
    800020da:	41f7579b          	sraiw	a5,a4,0x1f
    800020de:	0147d79b          	srliw	a5,a5,0x14
    800020e2:	9fb9                	addw	a5,a5,a4
    800020e4:	40c7d79b          	sraiw	a5,a5,0xc
    800020e8:	0791                	addi	a5,a5,4
    800020ea:	078a                	slli	a5,a5,0x2
    800020ec:	97aa                	add	a5,a5,a0
    800020ee:	0087aa03          	lw	s4,8(a5)
  if (rfs_r1block(rdev, n_block) != 0) {
    800020f2:	85d2                	mv	a1,s4
    800020f4:	854e                	mv	a0,s3
    800020f6:	eb2ff0ef          	jal	800017a8 <rfs_r1block>
    800020fa:	e12d                	bnez	a0,8000215c <rfs_add_direntry+0xb6>
  char *addr = (char *)rdev->iobuffer + dir->size % RFS_BLKSIZE;
    800020fc:	0109b503          	ld	a0,16(s3)
    80002100:	441c                	lw	a5,8(s0)
    80002102:	41f7d71b          	sraiw	a4,a5,0x1f
    80002106:	0147571b          	srliw	a4,a4,0x14
    8000210a:	9fb9                	addw	a5,a5,a4
    8000210c:	6685                	lui	a3,0x1
    8000210e:	16fd                	addi	a3,a3,-1 # fff <elf_fpread-0x7ffff001>
    80002110:	8ff5                	and	a5,a5,a3
    80002112:	9f99                	subw	a5,a5,a4
    80002114:	953e                	add	a0,a0,a5
  p_direntry->inum = inum;
    80002116:	01252023          	sw	s2,0(a0)
  strcpy(p_direntry->name, name);
    8000211a:	85a6                	mv	a1,s1
    8000211c:	0511                	addi	a0,a0,4
    8000211e:	0a4020ef          	jal	800041c2 <strcpy>
  if (rfs_w1block(rdev, n_block) != 0) {
    80002122:	85d2                	mv	a1,s4
    80002124:	854e                	mv	a0,s3
    80002126:	893ff0ef          	jal	800019b8 <rfs_w1block>
    8000212a:	e539                	bnez	a0,80002178 <rfs_add_direntry+0xd2>
  dir->size += sizeof(struct rfs_direntry);
    8000212c:	441c                	lw	a5,8(s0)
    8000212e:	0207879b          	addiw	a5,a5,32
    80002132:	c41c                	sw	a5,8(s0)
  if (rfs_write_back_vinode(dir) != 0) {
    80002134:	8522                	mv	a0,s0
    80002136:	a99ff0ef          	jal	80001bce <rfs_write_back_vinode>
    8000213a:	ed29                	bnez	a0,80002194 <rfs_add_direntry+0xee>
    8000213c:	7402                	ld	s0,32(sp)
    8000213e:	64e2                	ld	s1,24(sp)
    80002140:	6942                	ld	s2,16(sp)
    80002142:	69a2                	ld	s3,8(sp)
    80002144:	6a02                	ld	s4,0(sp)
}
    80002146:	70a2                	ld	ra,40(sp)
    80002148:	6145                	addi	sp,sp,48
    8000214a:	8082                	ret
    sprint("rfs_add_direntry: not a directory!\n");
    8000214c:	00006517          	auipc	a0,0x6
    80002150:	cd450513          	addi	a0,a0,-812 # 80007e20 <rfs_i_ops+0xda8>
    80002154:	29f020ef          	jal	80004bf2 <sprint>
    return -1;
    80002158:	557d                	li	a0,-1
    8000215a:	b7f5                	j	80002146 <rfs_add_direntry+0xa0>
    sprint("rfs_add_direntry: failed to read block %d!\n", n_block);
    8000215c:	85d2                	mv	a1,s4
    8000215e:	00006517          	auipc	a0,0x6
    80002162:	cea50513          	addi	a0,a0,-790 # 80007e48 <rfs_i_ops+0xdd0>
    80002166:	28d020ef          	jal	80004bf2 <sprint>
    return -1;
    8000216a:	557d                	li	a0,-1
    8000216c:	7402                	ld	s0,32(sp)
    8000216e:	64e2                	ld	s1,24(sp)
    80002170:	6942                	ld	s2,16(sp)
    80002172:	69a2                	ld	s3,8(sp)
    80002174:	6a02                	ld	s4,0(sp)
    80002176:	bfc1                	j	80002146 <rfs_add_direntry+0xa0>
    sprint("rfs_add_direntry: failed to write block %d!\n", n_block);
    80002178:	85d2                	mv	a1,s4
    8000217a:	00006517          	auipc	a0,0x6
    8000217e:	cfe50513          	addi	a0,a0,-770 # 80007e78 <rfs_i_ops+0xe00>
    80002182:	271020ef          	jal	80004bf2 <sprint>
    return -1;
    80002186:	557d                	li	a0,-1
    80002188:	7402                	ld	s0,32(sp)
    8000218a:	64e2                	ld	s1,24(sp)
    8000218c:	6942                	ld	s2,16(sp)
    8000218e:	69a2                	ld	s3,8(sp)
    80002190:	6a02                	ld	s4,0(sp)
    80002192:	bf55                	j	80002146 <rfs_add_direntry+0xa0>
    sprint("rfs_add_direntry: failed to write back parent dir inode!\n");
    80002194:	00006517          	auipc	a0,0x6
    80002198:	d1450513          	addi	a0,a0,-748 # 80007ea8 <rfs_i_ops+0xe30>
    8000219c:	257020ef          	jal	80004bf2 <sprint>
    return -1;
    800021a0:	557d                	li	a0,-1
    800021a2:	7402                	ld	s0,32(sp)
    800021a4:	64e2                	ld	s1,24(sp)
    800021a6:	6942                	ld	s2,16(sp)
    800021a8:	69a2                	ld	s3,8(sp)
    800021aa:	6a02                	ld	s4,0(sp)
    800021ac:	bf69                	j	80002146 <rfs_add_direntry+0xa0>

00000000800021ae <rfs_link>:
int rfs_link(struct vinode *parent, struct dentry *sub_dentry, struct vinode *link_node) {
    800021ae:	1141                	addi	sp,sp,-16
    800021b0:	e406                	sd	ra,8(sp)
    800021b2:	e022                	sd	s0,0(sp)
    800021b4:	8432                	mv	s0,a2
	link_node->nlinks++;
    800021b6:	4a1c                	lw	a5,16(a2)
    800021b8:	2785                	addiw	a5,a5,1
    800021ba:	ca1c                	sw	a5,16(a2)
	rfs_add_direntry(parent, sub_dentry->name, link_node->inum);
    800021bc:	4210                	lw	a2,0(a2)
    800021be:	ee9ff0ef          	jal	800020a6 <rfs_add_direntry>
	rfs_write_back_vinode(link_node);
    800021c2:	8522                	mv	a0,s0
    800021c4:	a0bff0ef          	jal	80001bce <rfs_write_back_vinode>
}
    800021c8:	4501                	li	a0,0
    800021ca:	60a2                	ld	ra,8(sp)
    800021cc:	6402                	ld	s0,0(sp)
    800021ce:	0141                	addi	sp,sp,16
    800021d0:	8082                	ret

00000000800021d2 <rfs_alloc_vinode>:
struct vinode *rfs_alloc_vinode(struct super_block *sb) {
    800021d2:	1141                	addi	sp,sp,-16
    800021d4:	e406                	sd	ra,8(sp)
  struct vinode *vinode = default_alloc_vinode(sb);
    800021d6:	634010ef          	jal	8000380a <default_alloc_vinode>
  vinode->i_ops = &rfs_i_ops;
    800021da:	00005797          	auipc	a5,0x5
    800021de:	e9e78793          	addi	a5,a5,-354 # 80007078 <rfs_i_ops>
    800021e2:	e93c                	sd	a5,80(a0)
}
    800021e4:	60a2                	ld	ra,8(sp)
    800021e6:	0141                	addi	sp,sp,16
    800021e8:	8082                	ret

00000000800021ea <rfs_update_vinode>:
int rfs_update_vinode(struct vinode *vinode) {
    800021ea:	1141                	addi	sp,sp,-16
    800021ec:	e406                	sd	ra,8(sp)
    800021ee:	e022                	sd	s0,0(sp)
    800021f0:	842a                	mv	s0,a0
  struct rfs_device *rdev = rfs_device_list[vinode->sb->s_dev->dev_id];
    800021f2:	653c                	ld	a5,72(a0)
    800021f4:	6f9c                	ld	a5,24(a5)
    800021f6:	5398                	lw	a4,32(a5)
    800021f8:	070e                	slli	a4,a4,0x3
    800021fa:	0000b797          	auipc	a5,0xb
    800021fe:	01678793          	addi	a5,a5,22 # 8000d210 <rfs_device_list>
    80002202:	97ba                	add	a5,a5,a4
  struct rfs_dinode *dinode = rfs_read_dinode(rdev, vinode->inum);
    80002204:	410c                	lw	a1,0(a0)
    80002206:	6388                	ld	a0,0(a5)
    80002208:	fbeff0ef          	jal	800019c6 <rfs_read_dinode>
  if (dinode == NULL) {
    8000220c:	c919                	beqz	a0,80002222 <rfs_update_vinode+0x38>
  vinode->size = dinode->size;
    8000220e:	411c                	lw	a5,0(a0)
    80002210:	c41c                	sw	a5,8(s0)
  vinode->nlinks = dinode->nlinks;
    80002212:	451c                	lw	a5,8(a0)
    80002214:	c81c                	sw	a5,16(s0)
  vinode->blocks = dinode->blocks;
    80002216:	455c                	lw	a5,12(a0)
    80002218:	c85c                	sw	a5,20(s0)
  vinode->type = dinode->type;
    8000221a:	415c                	lw	a5,4(a0)
    8000221c:	c45c                	sw	a5,12(s0)
  for (int i = 0; i < RFS_DIRECT_BLKNUM; ++i) {
    8000221e:	4701                	li	a4,0
    80002220:	a015                	j	80002244 <rfs_update_vinode+0x5a>
    sprint("rfs_update_vinode: failed to read disk inode!\n");
    80002222:	00006517          	auipc	a0,0x6
    80002226:	cc650513          	addi	a0,a0,-826 # 80007ee8 <rfs_i_ops+0xe70>
    8000222a:	1c9020ef          	jal	80004bf2 <sprint>
    return -1;
    8000222e:	557d                	li	a0,-1
    80002230:	a005                	j	80002250 <rfs_update_vinode+0x66>
    vinode->addrs[i] = dinode->addrs[i];
    80002232:	00470793          	addi	a5,a4,4
    80002236:	078a                	slli	a5,a5,0x2
    80002238:	00f506b3          	add	a3,a0,a5
    8000223c:	4294                	lw	a3,0(a3)
    8000223e:	97a2                	add	a5,a5,s0
    80002240:	c794                	sw	a3,8(a5)
  for (int i = 0; i < RFS_DIRECT_BLKNUM; ++i) {
    80002242:	2705                	addiw	a4,a4,1
    80002244:	47a5                	li	a5,9
    80002246:	fee7d6e3          	bge	a5,a4,80002232 <rfs_update_vinode+0x48>
  free_page(dinode);
    8000224a:	ce2fe0ef          	jal	8000072c <free_page>
  return 0;
    8000224e:	4501                	li	a0,0
}
    80002250:	60a2                	ld	ra,8(sp)
    80002252:	6402                	ld	s0,0(sp)
    80002254:	0141                	addi	sp,sp,16
    80002256:	8082                	ret

0000000080002258 <rfs_lookup>:
struct vinode *rfs_lookup(struct vinode *parent, struct dentry *sub_dentry) {
    80002258:	7139                	addi	sp,sp,-64
    8000225a:	fc06                	sd	ra,56(sp)
    8000225c:	f822                	sd	s0,48(sp)
    8000225e:	f426                	sd	s1,40(sp)
    80002260:	f04a                	sd	s2,32(sp)
    80002262:	ec4e                	sd	s3,24(sp)
    80002264:	e852                	sd	s4,16(sp)
    80002266:	e456                	sd	s5,8(sp)
    80002268:	89aa                	mv	s3,a0
    8000226a:	892e                	mv	s2,a1
  int total_direntrys = parent->size / sizeof(struct rfs_direntry);
    8000226c:	4504                	lw	s1,8(a0)
    8000226e:	8495                	srai	s1,s1,0x5
  struct rfs_device *rdev = rfs_device_list[parent->sb->s_dev->dev_id];
    80002270:	653c                	ld	a5,72(a0)
    80002272:	6f9c                	ld	a5,24(a5)
    80002274:	5398                	lw	a4,32(a5)
    80002276:	070e                	slli	a4,a4,0x3
    80002278:	0000b797          	auipc	a5,0xb
    8000227c:	f9878793          	addi	a5,a5,-104 # 8000d210 <rfs_device_list>
    80002280:	97ba                	add	a5,a5,a4
    80002282:	0007ba03          	ld	s4,0(a5)
  for (int i = 0; i < total_direntrys; ++i) {
    80002286:	4401                	li	s0,0
  struct rfs_direntry *p_direntry = NULL;
    80002288:	4a81                	li	s5,0
  for (int i = 0; i < total_direntrys; ++i) {
    8000228a:	a811                	j	8000229e <rfs_lookup+0x46>
    if (strcmp(p_direntry->name, sub_dentry->name) == 0) {  // found
    8000228c:	85ca                	mv	a1,s2
    8000228e:	004a8513          	addi	a0,s5,4
    80002292:	719010ef          	jal	800041aa <strcmp>
    80002296:	c915                	beqz	a0,800022ca <rfs_lookup+0x72>
    ++p_direntry;
    80002298:	020a8a93          	addi	s5,s5,32
  for (int i = 0; i < total_direntrys; ++i) {
    8000229c:	2405                	addiw	s0,s0,1
    8000229e:	04945763          	bge	s0,s1,800022ec <rfs_lookup+0x94>
    if (i % one_block_direntrys == 0) {  // read in the disk block at boundary
    800022a2:	07f47793          	andi	a5,s0,127
    800022a6:	f3fd                	bnez	a5,8000228c <rfs_lookup+0x34>
      rfs_r1block(rdev, parent->addrs[i / one_block_direntrys]);
    800022a8:	41f4579b          	sraiw	a5,s0,0x1f
    800022ac:	0197d79b          	srliw	a5,a5,0x19
    800022b0:	9fa1                	addw	a5,a5,s0
    800022b2:	4077d79b          	sraiw	a5,a5,0x7
    800022b6:	0791                	addi	a5,a5,4
    800022b8:	078a                	slli	a5,a5,0x2
    800022ba:	97ce                	add	a5,a5,s3
    800022bc:	478c                	lw	a1,8(a5)
    800022be:	8552                	mv	a0,s4
    800022c0:	ce8ff0ef          	jal	800017a8 <rfs_r1block>
      p_direntry = (struct rfs_direntry *)rdev->iobuffer;
    800022c4:	010a3a83          	ld	s5,16(s4)
    800022c8:	b7d1                	j	8000228c <rfs_lookup+0x34>
      child_vinode = rfs_alloc_vinode(parent->sb);
    800022ca:	0489b503          	ld	a0,72(s3)
    800022ce:	f05ff0ef          	jal	800021d2 <rfs_alloc_vinode>
    800022d2:	842a                	mv	s0,a0
      child_vinode->inum = p_direntry->inum;
    800022d4:	000aa783          	lw	a5,0(s5)
    800022d8:	c11c                	sw	a5,0(a0)
      if (rfs_update_vinode(child_vinode) != 0)
    800022da:	f11ff0ef          	jal	800021ea <rfs_update_vinode>
    800022de:	c901                	beqz	a0,800022ee <rfs_lookup+0x96>
        panic("rfs_lookup: read inode failed!");
    800022e0:	00006517          	auipc	a0,0x6
    800022e4:	c3850513          	addi	a0,a0,-968 # 80007f18 <rfs_i_ops+0xea0>
    800022e8:	1af020ef          	jal	80004c96 <do_panic>
  struct vinode *child_vinode = NULL;
    800022ec:	4401                	li	s0,0
}
    800022ee:	8522                	mv	a0,s0
    800022f0:	70e2                	ld	ra,56(sp)
    800022f2:	7442                	ld	s0,48(sp)
    800022f4:	74a2                	ld	s1,40(sp)
    800022f6:	7902                	ld	s2,32(sp)
    800022f8:	69e2                	ld	s3,24(sp)
    800022fa:	6a42                	ld	s4,16(sp)
    800022fc:	6aa2                	ld	s5,8(sp)
    800022fe:	6121                	addi	sp,sp,64
    80002300:	8082                	ret

0000000080002302 <rfs_create>:
struct vinode *rfs_create(struct vinode *parent, struct dentry *sub_dentry) {
    80002302:	7179                	addi	sp,sp,-48
    80002304:	f406                	sd	ra,40(sp)
    80002306:	f022                	sd	s0,32(sp)
    80002308:	ec26                	sd	s1,24(sp)
    8000230a:	e84a                	sd	s2,16(sp)
    8000230c:	e44e                	sd	s3,8(sp)
    8000230e:	e052                	sd	s4,0(sp)
    80002310:	89aa                	mv	s3,a0
    80002312:	8a2e                	mv	s4,a1
  struct rfs_device *rdev = rfs_device_list[parent->sb->s_dev->dev_id];
    80002314:	653c                	ld	a5,72(a0)
    80002316:	6f9c                	ld	a5,24(a5)
    80002318:	5398                	lw	a4,32(a5)
    8000231a:	070e                	slli	a4,a4,0x3
    8000231c:	0000b797          	auipc	a5,0xb
    80002320:	ef478793          	addi	a5,a5,-268 # 8000d210 <rfs_device_list>
    80002324:	97ba                	add	a5,a5,a4
    80002326:	0007b903          	ld	s2,0(a5)
  for (int i = 0; i < (RFS_BLKSIZE / RFS_INODESIZE * RFS_MAX_INODE_BLKNUM);
    8000232a:	4481                	li	s1,0
  struct rfs_dinode *free_dinode = NULL;
    8000232c:	4401                	li	s0,0
  for (int i = 0; i < (RFS_BLKSIZE / RFS_INODESIZE * RFS_MAX_INODE_BLKNUM);
    8000232e:	13f00793          	li	a5,319
    80002332:	0097cf63          	blt	a5,s1,80002350 <rfs_create+0x4e>
    free_dinode = rfs_read_dinode(rdev, i);
    80002336:	85a6                	mv	a1,s1
    80002338:	854a                	mv	a0,s2
    8000233a:	e8cff0ef          	jal	800019c6 <rfs_read_dinode>
    8000233e:	842a                	mv	s0,a0
    if (free_dinode->type == R_FREE) {  // found
    80002340:	4158                	lw	a4,4(a0)
    80002342:	4789                	li	a5,2
    80002344:	00f70763          	beq	a4,a5,80002352 <rfs_create+0x50>
    free_page(free_dinode);
    80002348:	be4fe0ef          	jal	8000072c <free_page>
       ++i) {
    8000234c:	2485                	addiw	s1,s1,1
    8000234e:	b7c5                	j	8000232e <rfs_create+0x2c>
  int free_inum = 0;
    80002350:	4481                	li	s1,0
  if (free_dinode == NULL)
    80002352:	cc31                	beqz	s0,800023ae <rfs_create+0xac>
	free_dinode->size = 0;
    80002354:	00042023          	sw	zero,0(s0)
	free_dinode->type = R_FILE;
    80002358:	00042223          	sw	zero,4(s0)
	free_dinode->nlinks = 1;
    8000235c:	4785                	li	a5,1
    8000235e:	c41c                	sw	a5,8(s0)
	free_dinode->blocks = 1;
    80002360:	c45c                	sw	a5,12(s0)
  free_dinode->addrs[0] = rfs_alloc_block(parent->sb);
    80002362:	0489b503          	ld	a0,72(s3)
    80002366:	8d3ff0ef          	jal	80001c38 <rfs_alloc_block>
    8000236a:	c808                	sw	a0,16(s0)
  rfs_write_dinode(rdev, free_dinode, free_inum);
    8000236c:	8626                	mv	a2,s1
    8000236e:	85a2                	mv	a1,s0
    80002370:	854a                	mv	a0,s2
    80002372:	f06ff0ef          	jal	80001a78 <rfs_write_dinode>
  free_page(free_dinode);
    80002376:	8522                	mv	a0,s0
    80002378:	bb4fe0ef          	jal	8000072c <free_page>
  struct vinode *new_vinode = rfs_alloc_vinode(parent->sb);
    8000237c:	0489b503          	ld	a0,72(s3)
    80002380:	e53ff0ef          	jal	800021d2 <rfs_alloc_vinode>
    80002384:	842a                	mv	s0,a0
  new_vinode->inum = free_inum;
    80002386:	c104                	sw	s1,0(a0)
  rfs_update_vinode(new_vinode);
    80002388:	e63ff0ef          	jal	800021ea <rfs_update_vinode>
  int result = rfs_add_direntry(parent, sub_dentry->name, free_inum);
    8000238c:	8626                	mv	a2,s1
    8000238e:	85d2                	mv	a1,s4
    80002390:	854e                	mv	a0,s3
    80002392:	d15ff0ef          	jal	800020a6 <rfs_add_direntry>
  if (result == -1) {
    80002396:	57fd                	li	a5,-1
    80002398:	02f50163          	beq	a0,a5,800023ba <rfs_create+0xb8>
}
    8000239c:	8522                	mv	a0,s0
    8000239e:	70a2                	ld	ra,40(sp)
    800023a0:	7402                	ld	s0,32(sp)
    800023a2:	64e2                	ld	s1,24(sp)
    800023a4:	6942                	ld	s2,16(sp)
    800023a6:	69a2                	ld	s3,8(sp)
    800023a8:	6a02                	ld	s4,0(sp)
    800023aa:	6145                	addi	sp,sp,48
    800023ac:	8082                	ret
    panic("rfs_create: no more free disk inode, we cannot create file.\n" );
    800023ae:	00006517          	auipc	a0,0x6
    800023b2:	b8a50513          	addi	a0,a0,-1142 # 80007f38 <rfs_i_ops+0xec0>
    800023b6:	0e1020ef          	jal	80004c96 <do_panic>
    sprint("rfs_create: rfs_add_direntry failed");
    800023ba:	00006517          	auipc	a0,0x6
    800023be:	bbe50513          	addi	a0,a0,-1090 # 80007f78 <rfs_i_ops+0xf00>
    800023c2:	031020ef          	jal	80004bf2 <sprint>
    return NULL;
    800023c6:	4401                	li	s0,0
    800023c8:	bfd1                	j	8000239c <rfs_create+0x9a>

00000000800023ca <rfs_mkdir>:

//
// make a new direntry named "sub_dentry->name" under the directory "parent",
// return the vfs inode of subdir being created.
//
struct vinode *rfs_mkdir(struct vinode *parent, struct dentry *sub_dentry) {
    800023ca:	7179                	addi	sp,sp,-48
    800023cc:	f406                	sd	ra,40(sp)
    800023ce:	f022                	sd	s0,32(sp)
    800023d0:	ec26                	sd	s1,24(sp)
    800023d2:	e84a                	sd	s2,16(sp)
    800023d4:	e44e                	sd	s3,8(sp)
    800023d6:	e052                	sd	s4,0(sp)
    800023d8:	89aa                	mv	s3,a0
    800023da:	8a2e                	mv	s4,a1
  struct rfs_device *rdev = rfs_device_list[parent->sb->s_dev->dev_id];
    800023dc:	653c                	ld	a5,72(a0)
    800023de:	6f9c                	ld	a5,24(a5)
    800023e0:	5398                	lw	a4,32(a5)
    800023e2:	070e                	slli	a4,a4,0x3
    800023e4:	0000b797          	auipc	a5,0xb
    800023e8:	e2c78793          	addi	a5,a5,-468 # 8000d210 <rfs_device_list>
    800023ec:	97ba                	add	a5,a5,a4
    800023ee:	0007b903          	ld	s2,0(a5)

  // ** find a free disk inode to store the file that is going to be created
  struct rfs_dinode *free_dinode = NULL;
  int free_inum = 0;
  for (int i = 0; i < (RFS_BLKSIZE / RFS_INODESIZE * RFS_MAX_INODE_BLKNUM); i++) {
    800023f2:	4481                	li	s1,0
  struct rfs_dinode *free_dinode = NULL;
    800023f4:	4401                	li	s0,0
  for (int i = 0; i < (RFS_BLKSIZE / RFS_INODESIZE * RFS_MAX_INODE_BLKNUM); i++) {
    800023f6:	13f00793          	li	a5,319
    800023fa:	0097cf63          	blt	a5,s1,80002418 <rfs_mkdir+0x4e>
    free_dinode = rfs_read_dinode(rdev, i);
    800023fe:	85a6                	mv	a1,s1
    80002400:	854a                	mv	a0,s2
    80002402:	dc4ff0ef          	jal	800019c6 <rfs_read_dinode>
    80002406:	842a                	mv	s0,a0
    if (free_dinode->type == R_FREE) {  // found
    80002408:	4158                	lw	a4,4(a0)
    8000240a:	4789                	li	a5,2
    8000240c:	00f70763          	beq	a4,a5,8000241a <rfs_mkdir+0x50>
      free_inum = i;
      break;
    }
    free_page(free_dinode);
    80002410:	b1cfe0ef          	jal	8000072c <free_page>
  for (int i = 0; i < (RFS_BLKSIZE / RFS_INODESIZE * RFS_MAX_INODE_BLKNUM); i++) {
    80002414:	2485                	addiw	s1,s1,1
    80002416:	b7c5                	j	800023f6 <rfs_mkdir+0x2c>
  int free_inum = 0;
    80002418:	4481                	li	s1,0
  }

  if (free_dinode == NULL)
    8000241a:	cc29                	beqz	s0,80002474 <rfs_mkdir+0xaa>
    panic( "rfs_mkdir: no more free disk inode, we cannot create directory.\n" );

  // initialize the states of the file being created
  free_dinode->size = 0;
    8000241c:	00042023          	sw	zero,0(s0)
  free_dinode->type = R_DIR;
    80002420:	4785                	li	a5,1
    80002422:	c05c                	sw	a5,4(s0)
  free_dinode->nlinks = 1;
    80002424:	c41c                	sw	a5,8(s0)
  free_dinode->blocks = 1;
    80002426:	c45c                	sw	a5,12(s0)
  // allocate a free block for the file
  free_dinode->addrs[0] = rfs_alloc_block(parent->sb);
    80002428:	0489b503          	ld	a0,72(s3)
    8000242c:	80dff0ef          	jal	80001c38 <rfs_alloc_block>
    80002430:	c808                	sw	a0,16(s0)

  // **  write the disk inode of file being created to disk
  rfs_write_dinode(rdev, free_dinode, free_inum);
    80002432:	8626                	mv	a2,s1
    80002434:	85a2                	mv	a1,s0
    80002436:	854a                	mv	a0,s2
    80002438:	e40ff0ef          	jal	80001a78 <rfs_write_dinode>
  free_page(free_dinode);
    8000243c:	8522                	mv	a0,s0
    8000243e:	aeefe0ef          	jal	8000072c <free_page>

  // ** add a direntry to the directory
  int result = rfs_add_direntry(parent, sub_dentry->name, free_inum);
    80002442:	8626                	mv	a2,s1
    80002444:	85d2                	mv	a1,s4
    80002446:	854e                	mv	a0,s3
    80002448:	c5fff0ef          	jal	800020a6 <rfs_add_direntry>
  if (result == -1) {
    8000244c:	57fd                	li	a5,-1
    8000244e:	02f50963          	beq	a0,a5,80002480 <rfs_mkdir+0xb6>
    sprint("rfs_mkdir: rfs_add_direntry failed");
    return NULL;
  }

  // ** allocate a new vinode
  struct vinode *sub_vinode = rfs_alloc_vinode(parent->sb);
    80002452:	0489b503          	ld	a0,72(s3)
    80002456:	d7dff0ef          	jal	800021d2 <rfs_alloc_vinode>
    8000245a:	842a                	mv	s0,a0
  sub_vinode->inum = free_inum;
    8000245c:	c104                	sw	s1,0(a0)
  rfs_update_vinode(sub_vinode);
    8000245e:	d8dff0ef          	jal	800021ea <rfs_update_vinode>

  return sub_vinode;
}
    80002462:	8522                	mv	a0,s0
    80002464:	70a2                	ld	ra,40(sp)
    80002466:	7402                	ld	s0,32(sp)
    80002468:	64e2                	ld	s1,24(sp)
    8000246a:	6942                	ld	s2,16(sp)
    8000246c:	69a2                	ld	s3,8(sp)
    8000246e:	6a02                	ld	s4,0(sp)
    80002470:	6145                	addi	sp,sp,48
    80002472:	8082                	ret
    panic( "rfs_mkdir: no more free disk inode, we cannot create directory.\n" );
    80002474:	00006517          	auipc	a0,0x6
    80002478:	b2c50513          	addi	a0,a0,-1236 # 80007fa0 <rfs_i_ops+0xf28>
    8000247c:	01b020ef          	jal	80004c96 <do_panic>
    sprint("rfs_mkdir: rfs_add_direntry failed");
    80002480:	00006517          	auipc	a0,0x6
    80002484:	b6850513          	addi	a0,a0,-1176 # 80007fe8 <rfs_i_ops+0xf70>
    80002488:	76a020ef          	jal	80004bf2 <sprint>
    return NULL;
    8000248c:	4401                	li	s0,0
    8000248e:	bfd1                	j	80002462 <rfs_mkdir+0x98>

0000000080002490 <rfs_get_superblock>:

/**** vfs-rfs file system type interface functions ****/
struct super_block *rfs_get_superblock(struct device *dev) {
    80002490:	7179                	addi	sp,sp,-48
    80002492:	f406                	sd	ra,40(sp)
    80002494:	f022                	sd	s0,32(sp)
    80002496:	ec26                	sd	s1,24(sp)
    80002498:	e84a                	sd	s2,16(sp)
    8000249a:	84aa                	mv	s1,a0
  struct rfs_device *rdev = rfs_device_list[dev->dev_id];
    8000249c:	5118                	lw	a4,32(a0)
    8000249e:	070e                	slli	a4,a4,0x3
    800024a0:	0000b797          	auipc	a5,0xb
    800024a4:	d7078793          	addi	a5,a5,-656 # 8000d210 <rfs_device_list>
    800024a8:	97ba                	add	a5,a5,a4
    800024aa:	0007b903          	ld	s2,0(a5)

  // read super block from ramdisk
  if (rfs_r1block(rdev, RFS_BLK_OFFSET_SUPER) != 0)
    800024ae:	4581                	li	a1,0
    800024b0:	854a                	mv	a0,s2
    800024b2:	af6ff0ef          	jal	800017a8 <rfs_r1block>
    800024b6:	ed2d                	bnez	a0,80002530 <rfs_get_superblock+0xa0>
    panic("RFS: failed to read superblock!\n");

  struct rfs_superblock d_sb;
  memcpy(&d_sb, rdev->iobuffer, sizeof(struct rfs_superblock));
    800024b8:	4641                	li	a2,16
    800024ba:	01093583          	ld	a1,16(s2)
    800024be:	850a                	mv	a0,sp
    800024c0:	45d010ef          	jal	8000411c <memcpy>

  // set the data for the vfs super block
  struct super_block *sb = alloc_page();
    800024c4:	b18fe0ef          	jal	800007dc <alloc_page>
    800024c8:	842a                	mv	s0,a0
  sb->magic = d_sb.magic;
    800024ca:	4702                	lw	a4,0(sp)
    800024cc:	c118                	sw	a4,0(a0)
  sb->size = d_sb.size;
    800024ce:	4792                	lw	a5,4(sp)
    800024d0:	c15c                	sw	a5,4(a0)
  sb->nblocks = d_sb.nblocks;
    800024d2:	47a2                	lw	a5,8(sp)
    800024d4:	c51c                	sw	a5,8(a0)
  sb->ninodes = d_sb.ninodes;
    800024d6:	47b2                	lw	a5,12(sp)
    800024d8:	c55c                	sw	a5,12(a0)
  sb->s_dev = dev;
    800024da:	ed04                	sd	s1,24(a0)

  if( sb->magic != RFS_MAGIC ) 
    800024dc:	67b1                	lui	a5,0xc
    800024de:	eaf78793          	addi	a5,a5,-337 # beaf <elf_fpread-0x7fff4151>
    800024e2:	04f71d63          	bne	a4,a5,8000253c <rfs_get_superblock+0xac>
    panic("rfs_get_superblock: wrong ramdisk device!\n");

  // build root dentry and root inode
  struct vinode *root_inode = rfs_alloc_vinode(sb);
    800024e6:	cedff0ef          	jal	800021d2 <rfs_alloc_vinode>
    800024ea:	84aa                	mv	s1,a0
  root_inode->inum = 0;
    800024ec:	00052023          	sw	zero,0(a0)
  rfs_update_vinode(root_inode);
    800024f0:	cfbff0ef          	jal	800021ea <rfs_update_vinode>

  struct dentry *root_dentry = alloc_vfs_dentry("/", root_inode, NULL);
    800024f4:	4601                	li	a2,0
    800024f6:	85a6                	mv	a1,s1
    800024f8:	00005517          	auipc	a0,0x5
    800024fc:	fa050513          	addi	a0,a0,-96 # 80007498 <rfs_i_ops+0x420>
    80002500:	33b000ef          	jal	8000303a <alloc_vfs_dentry>
  sb->s_root = root_dentry;
    80002504:	e808                	sd	a0,16(s0)

  // save the bitmap in the s_fs_info field
  if (rfs_r1block(rdev, RFS_BLK_OFFSET_BITMAP) != 0)
    80002506:	45ad                	li	a1,11
    80002508:	854a                	mv	a0,s2
    8000250a:	a9eff0ef          	jal	800017a8 <rfs_r1block>
    8000250e:	ed0d                	bnez	a0,80002548 <rfs_get_superblock+0xb8>
    panic("RFS: failed to read bitmap!\n");
  void *bitmap = alloc_page();
    80002510:	accfe0ef          	jal	800007dc <alloc_page>
    80002514:	84aa                	mv	s1,a0
  memcpy(bitmap, rdev->iobuffer, RFS_BLKSIZE);
    80002516:	6605                	lui	a2,0x1
    80002518:	01093583          	ld	a1,16(s2)
    8000251c:	401010ef          	jal	8000411c <memcpy>
  sb->s_fs_info = bitmap;
    80002520:	f004                	sd	s1,32(s0)

  return sb;
}
    80002522:	8522                	mv	a0,s0
    80002524:	70a2                	ld	ra,40(sp)
    80002526:	7402                	ld	s0,32(sp)
    80002528:	64e2                	ld	s1,24(sp)
    8000252a:	6942                	ld	s2,16(sp)
    8000252c:	6145                	addi	sp,sp,48
    8000252e:	8082                	ret
    panic("RFS: failed to read superblock!\n");
    80002530:	00006517          	auipc	a0,0x6
    80002534:	ae050513          	addi	a0,a0,-1312 # 80008010 <rfs_i_ops+0xf98>
    80002538:	75e020ef          	jal	80004c96 <do_panic>
    panic("rfs_get_superblock: wrong ramdisk device!\n");
    8000253c:	00006517          	auipc	a0,0x6
    80002540:	afc50513          	addi	a0,a0,-1284 # 80008038 <rfs_i_ops+0xfc0>
    80002544:	752020ef          	jal	80004c96 <do_panic>
    panic("RFS: failed to read bitmap!\n");
    80002548:	00006517          	auipc	a0,0x6
    8000254c:	b2050513          	addi	a0,a0,-1248 # 80008068 <rfs_i_ops+0xff0>
    80002550:	746020ef          	jal	80004c96 <do_panic>

0000000080002554 <insert_to_ready_queue>:
process* ready_queue_head = NULL;

//
// insert a process, proc, into the END of ready queue.
//
void insert_to_ready_queue( process* proc ) {
    80002554:	1141                	addi	sp,sp,-16
    80002556:	e406                	sd	ra,8(sp)
    80002558:	e022                	sd	s0,0(sp)
    8000255a:	842a                	mv	s0,a0
  sprint( "going to insert process %d to ready queue.\n", proc->pid );
    8000255c:	14053583          	ld	a1,320(a0)
    80002560:	00006517          	auipc	a0,0x6
    80002564:	b2850513          	addi	a0,a0,-1240 # 80008088 <rfs_i_ops+0x1010>
    80002568:	68a020ef          	jal	80004bf2 <sprint>
  // if the queue is empty in the beginning
  if( ready_queue_head == NULL ){
    8000256c:	0000e797          	auipc	a5,0xe
    80002570:	bc47b783          	ld	a5,-1084(a5) # 80010130 <ready_queue_head>
    80002574:	cb99                	beqz	a5,8000258a <insert_to_ready_queue+0x36>
  }

  // ready queue is not empty
  process *p;
  // browse the ready queue to see if proc is already in-queue
  for( p=ready_queue_head; p->queue_next!=NULL; p=p->queue_next )
    80002576:	873e                	mv	a4,a5
    80002578:	1587b783          	ld	a5,344(a5)
    8000257c:	c38d                	beqz	a5,8000259e <insert_to_ready_queue+0x4a>
    if( p == proc ) return;  //already in queue
    8000257e:	fe871ce3          	bne	a4,s0,80002576 <insert_to_ready_queue+0x22>
  p->queue_next = proc;
  proc->status = READY;
  proc->queue_next = NULL;

  return;
}
    80002582:	60a2                	ld	ra,8(sp)
    80002584:	6402                	ld	s0,0(sp)
    80002586:	0141                	addi	sp,sp,16
    80002588:	8082                	ret
    proc->status = READY;
    8000258a:	4785                	li	a5,1
    8000258c:	14f42423          	sw	a5,328(s0)
    proc->queue_next = NULL;
    80002590:	14043c23          	sd	zero,344(s0)
    ready_queue_head = proc;
    80002594:	0000e797          	auipc	a5,0xe
    80002598:	b887be23          	sd	s0,-1124(a5) # 80010130 <ready_queue_head>
    return;
    8000259c:	b7dd                	j	80002582 <insert_to_ready_queue+0x2e>
  if( p==proc ) return;
    8000259e:	fe8702e3          	beq	a4,s0,80002582 <insert_to_ready_queue+0x2e>
  p->queue_next = proc;
    800025a2:	14873c23          	sd	s0,344(a4)
  proc->status = READY;
    800025a6:	4785                	li	a5,1
    800025a8:	14f42423          	sw	a5,328(s0)
  proc->queue_next = NULL;
    800025ac:	14043c23          	sd	zero,344(s0)
  return;
    800025b0:	bfc9                	j	80002582 <insert_to_ready_queue+0x2e>

00000000800025b2 <schedule>:
// note: schedule() does not take care of previous current process. If the current
// process is still runnable, you should place it into the ready queue (by calling
// ready_queue_insert), and then call schedule().
//
extern process procs[NPROC];
void schedule() {
    800025b2:	1141                	addi	sp,sp,-16
    800025b4:	e406                	sd	ra,8(sp)
  if ( !ready_queue_head ){
    800025b6:	0000e797          	auipc	a5,0xe
    800025ba:	b7a7b783          	ld	a5,-1158(a5) # 80010130 <ready_queue_head>
    800025be:	c3c1                	beqz	a5,8000263e <schedule+0x8c>
    }else{
      panic( "Not handled: we should let system wait for unfinished processes.\n" );
    }
  }

  current = ready_queue_head;
    800025c0:	0000e717          	auipc	a4,0xe
    800025c4:	b6f73423          	sd	a5,-1176(a4) # 80010128 <current>
  assert( current->status == READY );
    800025c8:	1487a683          	lw	a3,328(a5)
    800025cc:	4705                	li	a4,1
    800025ce:	08e69c63          	bne	a3,a4,80002666 <schedule+0xb4>
  ready_queue_head = ready_queue_head->queue_next;
    800025d2:	1587b703          	ld	a4,344(a5)
    800025d6:	0000e697          	auipc	a3,0xe
    800025da:	b4e6bd23          	sd	a4,-1190(a3) # 80010130 <ready_queue_head>

  current->status = RUNNING;
    800025de:	4709                	li	a4,2
    800025e0:	14e7a423          	sw	a4,328(a5)
  sprint( "going to schedule process %d to run.\n", current->pid );
    800025e4:	1407b583          	ld	a1,320(a5)
    800025e8:	00006517          	auipc	a0,0x6
    800025ec:	bc050513          	addi	a0,a0,-1088 # 800081a8 <rfs_i_ops+0x1130>
    800025f0:	602020ef          	jal	80004bf2 <sprint>
  switch_to( current );
    800025f4:	0000e517          	auipc	a0,0xe
    800025f8:	b3453503          	ld	a0,-1228(a0) # 80010128 <current>
    800025fc:	bc2fe0ef          	jal	800009be <switch_to>
}
    80002600:	60a2                	ld	ra,8(sp)
    80002602:	0141                	addi	sp,sp,16
    80002604:	8082                	ret
    for( int i=0; i<NPROC; i++ )
    80002606:	2405                	addiw	s0,s0,1
    80002608:	47fd                	li	a5,31
    8000260a:	0287ce63          	blt	a5,s0,80002646 <schedule+0x94>
      if( (procs[i].status != FREE) && (procs[i].status != ZOMBIE) ){
    8000260e:	17000713          	li	a4,368
    80002612:	02e40733          	mul	a4,s0,a4
    80002616:	00008797          	auipc	a5,0x8
    8000261a:	dfa78793          	addi	a5,a5,-518 # 8000a410 <procs>
    8000261e:	97ba                	add	a5,a5,a4
    80002620:	1487a603          	lw	a2,328(a5)
    80002624:	d26d                	beqz	a2,80002606 <schedule+0x54>
    80002626:	4791                	li	a5,4
    80002628:	fcf60fe3          	beq	a2,a5,80002606 <schedule+0x54>
        sprint( "ready queue empty, but process %d is not in free/zombie state:%d\n", 
    8000262c:	85a2                	mv	a1,s0
    8000262e:	00006517          	auipc	a0,0x6
    80002632:	a8a50513          	addi	a0,a0,-1398 # 800080b8 <rfs_i_ops+0x1040>
    80002636:	5bc020ef          	jal	80004bf2 <sprint>
        should_shutdown = 0;
    8000263a:	4681                	li	a3,0
    8000263c:	b7e9                	j	80002606 <schedule+0x54>
    8000263e:	e022                	sd	s0,0(sp)
    for( int i=0; i<NPROC; i++ )
    80002640:	4401                	li	s0,0
    int should_shutdown = 1;
    80002642:	4685                	li	a3,1
    80002644:	b7d1                	j	80002608 <schedule+0x56>
    if( should_shutdown ){
    80002646:	ca91                	beqz	a3,8000265a <schedule+0xa8>
      sprint( "no more ready processes, system shutdown now.\n" );
    80002648:	00006517          	auipc	a0,0x6
    8000264c:	ab850513          	addi	a0,a0,-1352 # 80008100 <rfs_i_ops+0x1088>
    80002650:	5a2020ef          	jal	80004bf2 <sprint>
      shutdown( 0 );
    80002654:	4501                	li	a0,0
    80002656:	612020ef          	jal	80004c68 <shutdown>
      panic( "Not handled: we should let system wait for unfinished processes.\n" );
    8000265a:	00006517          	auipc	a0,0x6
    8000265e:	ad650513          	addi	a0,a0,-1322 # 80008130 <rfs_i_ops+0x10b8>
    80002662:	634020ef          	jal	80004c96 <do_panic>
    80002666:	e022                	sd	s0,0(sp)
  assert( current->status == READY );
    80002668:	00006697          	auipc	a3,0x6
    8000266c:	b1068693          	addi	a3,a3,-1264 # 80008178 <rfs_i_ops+0x1100>
    80002670:	04300613          	li	a2,67
    80002674:	00006597          	auipc	a1,0x6
    80002678:	b2458593          	addi	a1,a1,-1244 # 80008198 <rfs_i_ops+0x1120>
    8000267c:	00005517          	auipc	a0,0x5
    80002680:	10c50513          	addi	a0,a0,268 # 80007788 <rfs_i_ops+0x710>
    80002684:	56e020ef          	jal	80004bf2 <sprint>
    80002688:	6541                	lui	a0,0x10
    8000268a:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    8000268c:	586020ef          	jal	80004c12 <poweroff>

0000000080002690 <handle_syscall>:
#include "spike_interface/spike_utils.h"

//
// handling the syscalls. will call do_syscall() defined in kernel/syscall.c
//
static void handle_syscall(trapframe *tf) {
    80002690:	1141                	addi	sp,sp,-16
    80002692:	e406                	sd	ra,8(sp)
    80002694:	e022                	sd	s0,0(sp)
    80002696:	842a                	mv	s0,a0
  // tf->epc points to the address that our computer will jump to after the trap handling.
  // for a syscall, we should return to the NEXT instruction after its handling.
  // in RV64G, each instruction occupies exactly 32 bits (i.e., 4 Bytes)
  tf->epc += 4;
    80002698:	10853783          	ld	a5,264(a0)
    8000269c:	0791                	addi	a5,a5,4
    8000269e:	10f53423          	sd	a5,264(a0)

  // TODO (lab1_1): remove the panic call below, and call do_syscall (defined in
  // kernel/syscall.c) to conduct real operations of the kernel side for a syscall.
  // IMPORTANT: return value should be returned to user app, or else, you will encounter
  // problems in later experiments!
	tf->regs.a0 = do_syscall(tf->regs.a0,
    800026a2:	08053883          	ld	a7,128(a0)
    800026a6:	07853803          	ld	a6,120(a0)
    800026aa:	793c                	ld	a5,112(a0)
    800026ac:	7538                	ld	a4,104(a0)
    800026ae:	7134                	ld	a3,96(a0)
    800026b0:	6d30                	ld	a2,88(a0)
    800026b2:	692c                	ld	a1,80(a0)
    800026b4:	6528                	ld	a0,72(a0)
    800026b6:	5d0000ef          	jal	80002c86 <do_syscall>
    800026ba:	e428                	sd	a0,72(s0)
		tf->regs.a4,
		tf->regs.a5,
		tf->regs.a6,
		tf->regs.a7);

}
    800026bc:	60a2                	ld	ra,8(sp)
    800026be:	6402                	ld	s0,0(sp)
    800026c0:	0141                	addi	sp,sp,16
    800026c2:	8082                	ret

00000000800026c4 <handle_mtimer_trap>:
// global variable that store the recorded "ticks". added @lab1_3
static uint64 g_ticks = 0;
//
// added @lab1_3
//
void handle_mtimer_trap() {
    800026c4:	1141                	addi	sp,sp,-16
    800026c6:	e406                	sd	ra,8(sp)
    800026c8:	e022                	sd	s0,0(sp)
  sprint("Ticks %d\n", g_ticks);
    800026ca:	0000e417          	auipc	s0,0xe
    800026ce:	a6e40413          	addi	s0,s0,-1426 # 80010138 <g_ticks>
    800026d2:	600c                	ld	a1,0(s0)
    800026d4:	00006517          	auipc	a0,0x6
    800026d8:	afc50513          	addi	a0,a0,-1284 # 800081d0 <rfs_i_ops+0x1158>
    800026dc:	516020ef          	jal	80004bf2 <sprint>
  // TODO (lab1_3): increase g_ticks to record this "tick", and then clear the "SIP"
  // field in sip register.
  // hint: use write_csr to disable the SIP_SSIP bit in sip.
  g_ticks++;
    800026e0:	601c                	ld	a5,0(s0)
    800026e2:	0785                	addi	a5,a5,1
    800026e4:	e01c                	sd	a5,0(s0)
  write_csr(sip, read_csr(sip) & ~SIP_SSIP);
    800026e6:	144027f3          	csrr	a5,sip
    800026ea:	9bf5                	andi	a5,a5,-3
    800026ec:	14479073          	csrw	sip,a5

}
    800026f0:	60a2                	ld	ra,8(sp)
    800026f2:	6402                	ld	s0,0(sp)
    800026f4:	0141                	addi	sp,sp,16
    800026f6:	8082                	ret

00000000800026f8 <handle_user_page_fault>:
//
// the page fault handler. added @lab2_3. parameters:
// sepc: the pc when fault happens;
// stval: the virtual address that causes pagefault when being accessed.
//
void handle_user_page_fault(uint64 mcause, uint64 sepc, uint64 stval) {
    800026f8:	1101                	addi	sp,sp,-32
    800026fa:	ec06                	sd	ra,24(sp)
    800026fc:	e822                	sd	s0,16(sp)
    800026fe:	e426                	sd	s1,8(sp)
    80002700:	84aa                	mv	s1,a0
    80002702:	8432                	mv	s0,a2
  sprint("handle_page_fault: %lx\n", stval);
    80002704:	85b2                	mv	a1,a2
    80002706:	00006517          	auipc	a0,0x6
    8000270a:	ada50513          	addi	a0,a0,-1318 # 800081e0 <rfs_i_ops+0x1168>
    8000270e:	4e4020ef          	jal	80004bf2 <sprint>
  switch (mcause) {
    80002712:	47bd                	li	a5,15
    80002714:	00f48d63          	beq	s1,a5,8000272e <handle_user_page_fault+0x36>
		}

		break;
	}
    default:
      sprint("unknown page fault.\n");
    80002718:	00006517          	auipc	a0,0x6
    8000271c:	b1850513          	addi	a0,a0,-1256 # 80008230 <rfs_i_ops+0x11b8>
    80002720:	4d2020ef          	jal	80004bf2 <sprint>
      break;
  }
}
    80002724:	60e2                	ld	ra,24(sp)
    80002726:	6442                	ld	s0,16(sp)
    80002728:	64a2                	ld	s1,8(sp)
    8000272a:	6105                	addi	sp,sp,32
    8000272c:	8082                	ret
    8000272e:	e04a                	sd	s2,0(sp)
		struct Page *new_page = alloc_page();
    80002730:	8acfe0ef          	jal	800007dc <alloc_page>
    80002734:	84aa                	mv	s1,a0
		if (!new_page) {
    80002736:	c51d                	beqz	a0,80002764 <handle_user_page_fault+0x6c>
		if (map_pages(current->pagetable, ROUNDDOWN(stval,PGSIZE), PGSIZE, (uint64)new_page, prot_to_type(
    80002738:	0000e797          	auipc	a5,0xe
    8000273c:	9f07b783          	ld	a5,-1552(a5) # 80010128 <current>
    80002740:	0087b903          	ld	s2,8(a5)
    80002744:	4585                	li	a1,1
    80002746:	450d                	li	a0,3
    80002748:	0ec010ef          	jal	80003834 <prot_to_type>
    8000274c:	0005071b          	sext.w	a4,a0
    80002750:	86a6                	mv	a3,s1
    80002752:	6605                	lui	a2,0x1
    80002754:	75fd                	lui	a1,0xfffff
    80002756:	8de1                	and	a1,a1,s0
    80002758:	854a                	mv	a0,s2
    8000275a:	1a2010ef          	jal	800038fc <map_pages>
    8000275e:	e909                	bnez	a0,80002770 <handle_user_page_fault+0x78>
    80002760:	6902                	ld	s2,0(sp)
    80002762:	b7c9                	j	80002724 <handle_user_page_fault+0x2c>
			panic("Page allocation failed.\n");
    80002764:	00006517          	auipc	a0,0x6
    80002768:	a9450513          	addi	a0,a0,-1388 # 800081f8 <rfs_i_ops+0x1180>
    8000276c:	52a020ef          	jal	80004c96 <do_panic>
			panic("Page mapping failed.\n");
    80002770:	00006517          	auipc	a0,0x6
    80002774:	aa850513          	addi	a0,a0,-1368 # 80008218 <rfs_i_ops+0x11a0>
    80002778:	51e020ef          	jal	80004c96 <do_panic>

000000008000277c <rrsched>:
void rrsched() {
  // TODO (lab3_3): implements round-robin scheduling.
  // hint: increase the tick_count member of current process by one, if it is bigger than
  // TIME_SLICE_LEN (means it has consumed its time slice), change its status into READY,
  // place it in the rear of ready queue, and finally schedule next process to run.
	current->tick_count++;
    8000277c:	0000e517          	auipc	a0,0xe
    80002780:	9ac53503          	ld	a0,-1620(a0) # 80010128 <current>
    80002784:	16052783          	lw	a5,352(a0)
    80002788:	2785                	addiw	a5,a5,1
    8000278a:	0007871b          	sext.w	a4,a5
    8000278e:	16f52023          	sw	a5,352(a0)
	if (current->tick_count >= TIME_SLICE_LEN) {
    80002792:	4785                	li	a5,1
    80002794:	00e7c363          	blt	a5,a4,8000279a <rrsched+0x1e>
    80002798:	8082                	ret
void rrsched() {
    8000279a:	1141                	addi	sp,sp,-16
    8000279c:	e406                	sd	ra,8(sp)
		current->tick_count = 0;
    8000279e:	16052023          	sw	zero,352(a0)
		current->status = READY;
    800027a2:	14f52423          	sw	a5,328(a0)
		insert_to_ready_queue(current);
    800027a6:	dafff0ef          	jal	80002554 <insert_to_ready_queue>
		schedule();
    800027aa:	e09ff0ef          	jal	800025b2 <schedule>
	}

}
    800027ae:	60a2                	ld	ra,8(sp)
    800027b0:	0141                	addi	sp,sp,16
    800027b2:	8082                	ret

00000000800027b4 <smode_trap_handler>:

//
// kernel/smode_trap.S will pass control to smode_trap_handler, when a trap happens
// in S-mode.
//
void smode_trap_handler(void) {
    800027b4:	1141                	addi	sp,sp,-16
    800027b6:	e406                	sd	ra,8(sp)
  // make sure we are in User mode before entering the trap handling.
  // we will consider other previous case in lab1_3 (interrupt).
  if ((read_csr(sstatus) & SSTATUS_SPP) != 0) panic("usertrap: not from user mode");
    800027b8:	100027f3          	csrr	a5,sstatus
    800027bc:	1007f793          	andi	a5,a5,256
    800027c0:	e3a9                	bnez	a5,80002802 <smode_trap_handler+0x4e>

  assert(current);
    800027c2:	0000e797          	auipc	a5,0xe
    800027c6:	9667b783          	ld	a5,-1690(a5) # 80010128 <current>
    800027ca:	c3b1                	beqz	a5,8000280e <smode_trap_handler+0x5a>
  // save user process counter.
  current->trapframe->epc = read_csr(sepc);
    800027cc:	141026f3          	csrr	a3,sepc
    800027d0:	6b98                	ld	a4,16(a5)
    800027d2:	10d73423          	sd	a3,264(a4)

  // if the cause of trap is syscall from user application.
  // read_csr() and CAUSE_USER_ECALL are macros defined in kernel/riscv.h
  uint64 cause = read_csr(scause);
    800027d6:	14202573          	csrr	a0,scause

  // use switch-case instead of if-else, as there are many cases since lab2_3.
  switch (cause) {
    800027da:	473d                	li	a4,15
    800027dc:	06e50063          	beq	a0,a4,8000283c <smode_trap_handler+0x88>
    800027e0:	06a76563          	bltu	a4,a0,8000284a <smode_trap_handler+0x96>
    800027e4:	4721                	li	a4,8
    800027e6:	04e51863          	bne	a0,a4,80002836 <smode_trap_handler+0x82>
    case CAUSE_USER_ECALL:
      handle_syscall(current->trapframe);
    800027ea:	6b88                	ld	a0,16(a5)
    800027ec:	ea5ff0ef          	jal	80002690 <handle_syscall>
      panic( "unexpected exception happened.\n" );
      break;
  }

  // continue (come back to) the execution of current process.
  switch_to(current);
    800027f0:	0000e517          	auipc	a0,0xe
    800027f4:	93853503          	ld	a0,-1736(a0) # 80010128 <current>
    800027f8:	9c6fe0ef          	jal	800009be <switch_to>
}
    800027fc:	60a2                	ld	ra,8(sp)
    800027fe:	0141                	addi	sp,sp,16
    80002800:	8082                	ret
  if ((read_csr(sstatus) & SSTATUS_SPP) != 0) panic("usertrap: not from user mode");
    80002802:	00006517          	auipc	a0,0x6
    80002806:	a4650513          	addi	a0,a0,-1466 # 80008248 <rfs_i_ops+0x11d0>
    8000280a:	48c020ef          	jal	80004c96 <do_panic>
  assert(current);
    8000280e:	00006697          	auipc	a3,0x6
    80002812:	a5a68693          	addi	a3,a3,-1446 # 80008268 <rfs_i_ops+0x11f0>
    80002816:	07500613          	li	a2,117
    8000281a:	00006597          	auipc	a1,0x6
    8000281e:	a5658593          	addi	a1,a1,-1450 # 80008270 <rfs_i_ops+0x11f8>
    80002822:	00005517          	auipc	a0,0x5
    80002826:	f6650513          	addi	a0,a0,-154 # 80007788 <rfs_i_ops+0x710>
    8000282a:	3c8020ef          	jal	80004bf2 <sprint>
    8000282e:	6541                	lui	a0,0x10
    80002830:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    80002832:	3e0020ef          	jal	80004c12 <poweroff>
  switch (cause) {
    80002836:	47b5                	li	a5,13
    80002838:	02f51363          	bne	a0,a5,8000285e <smode_trap_handler+0xaa>
      handle_user_page_fault(cause, read_csr(sepc), read_csr(stval));
    8000283c:	141025f3          	csrr	a1,sepc
    80002840:	14302673          	csrr	a2,stval
    80002844:	eb5ff0ef          	jal	800026f8 <handle_user_page_fault>
      break;
    80002848:	b765                	j	800027f0 <smode_trap_handler+0x3c>
  switch (cause) {
    8000284a:	57fd                	li	a5,-1
    8000284c:	17fe                	slli	a5,a5,0x3f
    8000284e:	0785                	addi	a5,a5,1
    80002850:	00f51763          	bne	a0,a5,8000285e <smode_trap_handler+0xaa>
      handle_mtimer_trap();
    80002854:	e71ff0ef          	jal	800026c4 <handle_mtimer_trap>
      rrsched();
    80002858:	f25ff0ef          	jal	8000277c <rrsched>
      break;
    8000285c:	bf51                	j	800027f0 <smode_trap_handler+0x3c>
      sprint("smode_trap_handler(): unexpected scause %p\n", read_csr(scause));
    8000285e:	142025f3          	csrr	a1,scause
    80002862:	00006517          	auipc	a0,0x6
    80002866:	a1e50513          	addi	a0,a0,-1506 # 80008280 <rfs_i_ops+0x1208>
    8000286a:	388020ef          	jal	80004bf2 <sprint>
      sprint("            sepc=%p stval=%p\n", read_csr(sepc), read_csr(stval));
    8000286e:	141025f3          	csrr	a1,sepc
    80002872:	14302673          	csrr	a2,stval
    80002876:	00006517          	auipc	a0,0x6
    8000287a:	a3a50513          	addi	a0,a0,-1478 # 800082b0 <rfs_i_ops+0x1238>
    8000287e:	374020ef          	jal	80004bf2 <sprint>
      panic( "unexpected exception happened.\n" );
    80002882:	00006517          	auipc	a0,0x6
    80002886:	a4e50513          	addi	a0,a0,-1458 # 800082d0 <rfs_i_ops+0x1258>
    8000288a:	40c020ef          	jal	80004c96 <do_panic>

000000008000288e <sys_user_print>:
#include "spike_interface/spike_utils.h"

//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char* buf, size_t n) {
    8000288e:	1141                	addi	sp,sp,-16
    80002890:	e406                	sd	ra,8(sp)
  // buf is now an address in user space of the given app's user stack,
  // so we have to transfer it into phisical address (kernel is running in direct mapping).
  assert( current );
    80002892:	0000e797          	auipc	a5,0xe
    80002896:	8967b783          	ld	a5,-1898(a5) # 80010128 <current>
    8000289a:	cb99                	beqz	a5,800028b0 <sys_user_print+0x22>
    8000289c:	85aa                	mv	a1,a0
  char* pa = (char*)user_va_to_pa((pagetable_t)(current->pagetable), (void*)buf);
    8000289e:	6788                	ld	a0,8(a5)
    800028a0:	1c6010ef          	jal	80003a66 <user_va_to_pa>
  sprint(pa);
    800028a4:	34e020ef          	jal	80004bf2 <sprint>
  return 0;
}
    800028a8:	4501                	li	a0,0
    800028aa:	60a2                	ld	ra,8(sp)
    800028ac:	0141                	addi	sp,sp,16
    800028ae:	8082                	ret
  assert( current );
    800028b0:	00006697          	auipc	a3,0x6
    800028b4:	9b868693          	addi	a3,a3,-1608 # 80008268 <rfs_i_ops+0x11f0>
    800028b8:	4669                	li	a2,26
    800028ba:	00006597          	auipc	a1,0x6
    800028be:	a3e58593          	addi	a1,a1,-1474 # 800082f8 <rfs_i_ops+0x1280>
    800028c2:	00005517          	auipc	a0,0x5
    800028c6:	ec650513          	addi	a0,a0,-314 # 80007788 <rfs_i_ops+0x710>
    800028ca:	328020ef          	jal	80004bf2 <sprint>
    800028ce:	6541                	lui	a0,0x10
    800028d0:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    800028d2:	340020ef          	jal	80004c12 <poweroff>

00000000800028d6 <sys_user_exit>:

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code) {
    800028d6:	1141                	addi	sp,sp,-16
    800028d8:	e406                	sd	ra,8(sp)
    800028da:	85aa                	mv	a1,a0
  sprint("User exit with code:%d.\n", code);
    800028dc:	00006517          	auipc	a0,0x6
    800028e0:	a3450513          	addi	a0,a0,-1484 # 80008310 <rfs_i_ops+0x1298>
    800028e4:	30e020ef          	jal	80004bf2 <sprint>
  // reclaim the current process, and reschedule. added @lab3_1
  free_process( current );
    800028e8:	0000e517          	auipc	a0,0xe
    800028ec:	84053503          	ld	a0,-1984(a0) # 80010128 <current>
    800028f0:	b24fe0ef          	jal	80000c14 <free_process>
  schedule();
    800028f4:	cbfff0ef          	jal	800025b2 <schedule>
  return 0;
}
    800028f8:	4501                	li	a0,0
    800028fa:	60a2                	ld	ra,8(sp)
    800028fc:	0141                	addi	sp,sp,16
    800028fe:	8082                	ret

0000000080002900 <sys_user_allocate_page>:

//
// maybe, the simplest implementation of malloc in the world ... added @lab2_2
//
uint64 sys_user_allocate_page() {
    80002900:	1101                	addi	sp,sp,-32
    80002902:	ec06                	sd	ra,24(sp)
    80002904:	e822                	sd	s0,16(sp)
    80002906:	e426                	sd	s1,8(sp)
    80002908:	e04a                	sd	s2,0(sp)
  void* pa = alloc_page();
    8000290a:	ed3fd0ef          	jal	800007dc <alloc_page>
    8000290e:	842a                	mv	s0,a0
  uint64 va;
  // if there are previously reclaimed pages, use them first (this does not change the
  // size of the heap)
  if (current->user_heap.free_pages_count > 0) {
    80002910:	0000e717          	auipc	a4,0xe
    80002914:	81873703          	ld	a4,-2024(a4) # 80010128 <current>
    80002918:	13872783          	lw	a5,312(a4)
    8000291c:	c3a9                	beqz	a5,8000295e <sys_user_allocate_page+0x5e>
    va =  current->user_heap.free_pages_address[--current->user_heap.free_pages_count];
    8000291e:	37fd                	addiw	a5,a5,-1
    80002920:	12f72c23          	sw	a5,312(a4)
    80002924:	1782                	slli	a5,a5,0x20
    80002926:	9381                	srli	a5,a5,0x20
    80002928:	0799                	addi	a5,a5,6
    8000292a:	078e                	slli	a5,a5,0x3
    8000292c:	97ba                	add	a5,a5,a4
    8000292e:	6784                	ld	s1,8(a5)
    assert(va < current->user_heap.heap_top);
    80002930:	771c                	ld	a5,40(a4)
    80002932:	02f4ee63          	bltu	s1,a5,8000296e <sys_user_allocate_page+0x6e>
    80002936:	00006697          	auipc	a3,0x6
    8000293a:	9fa68693          	addi	a3,a3,-1542 # 80008330 <rfs_i_ops+0x12b8>
    8000293e:	03500613          	li	a2,53
    80002942:	00006597          	auipc	a1,0x6
    80002946:	9b658593          	addi	a1,a1,-1610 # 800082f8 <rfs_i_ops+0x1280>
    8000294a:	00005517          	auipc	a0,0x5
    8000294e:	e3e50513          	addi	a0,a0,-450 # 80007788 <rfs_i_ops+0x710>
    80002952:	2a0020ef          	jal	80004bf2 <sprint>
    80002956:	6541                	lui	a0,0x10
    80002958:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    8000295a:	2b8020ef          	jal	80004c12 <poweroff>
  } else {
    // otherwise, allocate a new page (this increases the size of the heap by one page)
    va = current->user_heap.heap_top;
    8000295e:	7704                	ld	s1,40(a4)
    current->user_heap.heap_top += PGSIZE;
    80002960:	6785                	lui	a5,0x1
    80002962:	97a6                	add	a5,a5,s1
    80002964:	f71c                	sd	a5,40(a4)

    current->mapped_info[HEAP_SEGMENT].npages++;
    80002966:	6f14                	ld	a3,24(a4)
    80002968:	5e9c                	lw	a5,56(a3)
    8000296a:	2785                	addiw	a5,a5,1 # 1001 <elf_fpread-0x7fffefff>
    8000296c:	de9c                	sw	a5,56(a3)
  }
  user_vm_map((pagetable_t)current->pagetable, va, PGSIZE, (uint64)pa,
    8000296e:	00873903          	ld	s2,8(a4)
         prot_to_type(PROT_WRITE | PROT_READ, 1));
    80002972:	4585                	li	a1,1
    80002974:	450d                	li	a0,3
    80002976:	6bf000ef          	jal	80003834 <prot_to_type>
  user_vm_map((pagetable_t)current->pagetable, va, PGSIZE, (uint64)pa,
    8000297a:	0005071b          	sext.w	a4,a0
    8000297e:	86a2                	mv	a3,s0
    80002980:	6605                	lui	a2,0x1
    80002982:	85a6                	mv	a1,s1
    80002984:	854a                	mv	a0,s2
    80002986:	0fc010ef          	jal	80003a82 <user_vm_map>

  return va;
}
    8000298a:	8526                	mv	a0,s1
    8000298c:	60e2                	ld	ra,24(sp)
    8000298e:	6442                	ld	s0,16(sp)
    80002990:	64a2                	ld	s1,8(sp)
    80002992:	6902                	ld	s2,0(sp)
    80002994:	6105                	addi	sp,sp,32
    80002996:	8082                	ret

0000000080002998 <sys_user_free_page>:

//
// reclaim a page, indicated by "va". added @lab2_2
//
uint64 sys_user_free_page(uint64 va) {
    80002998:	1101                	addi	sp,sp,-32
    8000299a:	ec06                	sd	ra,24(sp)
    8000299c:	e822                	sd	s0,16(sp)
    8000299e:	e426                	sd	s1,8(sp)
    800029a0:	842a                	mv	s0,a0
  user_vm_unmap((pagetable_t)current->pagetable, va, PGSIZE, 1);
    800029a2:	0000d497          	auipc	s1,0xd
    800029a6:	78648493          	addi	s1,s1,1926 # 80010128 <current>
    800029aa:	609c                	ld	a5,0(s1)
    800029ac:	4685                	li	a3,1
    800029ae:	6605                	lui	a2,0x1
    800029b0:	85aa                	mv	a1,a0
    800029b2:	6788                	ld	a0,8(a5)
    800029b4:	0ea010ef          	jal	80003a9e <user_vm_unmap>
  // add the reclaimed page to the free page list
  current->user_heap.free_pages_address[current->user_heap.free_pages_count++] = va;
    800029b8:	6098                	ld	a4,0(s1)
    800029ba:	13872783          	lw	a5,312(a4)
    800029be:	0017869b          	addiw	a3,a5,1
    800029c2:	12d72c23          	sw	a3,312(a4)
    800029c6:	1782                	slli	a5,a5,0x20
    800029c8:	9381                	srli	a5,a5,0x20
    800029ca:	0799                	addi	a5,a5,6
    800029cc:	078e                	slli	a5,a5,0x3
    800029ce:	973e                	add	a4,a4,a5
    800029d0:	e700                	sd	s0,8(a4)
  return 0;
}
    800029d2:	4501                	li	a0,0
    800029d4:	60e2                	ld	ra,24(sp)
    800029d6:	6442                	ld	s0,16(sp)
    800029d8:	64a2                	ld	s1,8(sp)
    800029da:	6105                	addi	sp,sp,32
    800029dc:	8082                	ret

00000000800029de <sys_user_fork>:

//
// kerenl entry point of naive_fork
//
ssize_t sys_user_fork() {
    800029de:	1141                	addi	sp,sp,-16
    800029e0:	e406                	sd	ra,8(sp)
  sprint("User call fork.\n");
    800029e2:	00006517          	auipc	a0,0x6
    800029e6:	97650513          	addi	a0,a0,-1674 # 80008358 <rfs_i_ops+0x12e0>
    800029ea:	208020ef          	jal	80004bf2 <sprint>
  return do_fork( current );
    800029ee:	0000d517          	auipc	a0,0xd
    800029f2:	73a53503          	ld	a0,1850(a0) # 80010128 <current>
    800029f6:	a28fe0ef          	jal	80000c1e <do_fork>
}
    800029fa:	60a2                	ld	ra,8(sp)
    800029fc:	0141                	addi	sp,sp,16
    800029fe:	8082                	ret

0000000080002a00 <sys_user_yield>:

//
// kerenl entry point of yield. added @lab3_2
//
ssize_t sys_user_yield() {
    80002a00:	1141                	addi	sp,sp,-16
    80002a02:	e406                	sd	ra,8(sp)
  // TODO (lab3_2): implment the syscall of yield.
  // hint: the functionality of yield is to give up the processor. therefore,
  // we should set the status of currently running process to READY, insert it in
  // the rear of ready queue, and finally, schedule a READY process to run.
	current->status = READY;
    80002a04:	0000d517          	auipc	a0,0xd
    80002a08:	72453503          	ld	a0,1828(a0) # 80010128 <current>
    80002a0c:	4785                	li	a5,1
    80002a0e:	14f52423          	sw	a5,328(a0)
	insert_to_ready_queue(current);
    80002a12:	b43ff0ef          	jal	80002554 <insert_to_ready_queue>
	schedule();
    80002a16:	b9dff0ef          	jal	800025b2 <schedule>

  return 0;
}
    80002a1a:	4501                	li	a0,0
    80002a1c:	60a2                	ld	ra,8(sp)
    80002a1e:	0141                	addi	sp,sp,16
    80002a20:	8082                	ret

0000000080002a22 <sys_user_open>:

//
// open file
//
ssize_t sys_user_open(char *pathva, int flags) {
    80002a22:	1141                	addi	sp,sp,-16
    80002a24:	e406                	sd	ra,8(sp)
    80002a26:	e022                	sd	s0,0(sp)
    80002a28:	842e                	mv	s0,a1
  char* pathpa = (char*)user_va_to_pa((pagetable_t)(current->pagetable), pathva);
    80002a2a:	85aa                	mv	a1,a0
    80002a2c:	0000d797          	auipc	a5,0xd
    80002a30:	6fc7b783          	ld	a5,1788(a5) # 80010128 <current>
    80002a34:	6788                	ld	a0,8(a5)
    80002a36:	030010ef          	jal	80003a66 <user_va_to_pa>
  return do_open(pathpa, flags);
    80002a3a:	85a2                	mv	a1,s0
    80002a3c:	e1cfe0ef          	jal	80001058 <do_open>
}
    80002a40:	60a2                	ld	ra,8(sp)
    80002a42:	6402                	ld	s0,0(sp)
    80002a44:	0141                	addi	sp,sp,16
    80002a46:	8082                	ret

0000000080002a48 <sys_user_read>:

//
// read file
//
ssize_t sys_user_read(int fd, char *bufva, uint64 count) {
    80002a48:	7139                	addi	sp,sp,-64
    80002a4a:	fc06                	sd	ra,56(sp)
    80002a4c:	f822                	sd	s0,48(sp)
    80002a4e:	f426                	sd	s1,40(sp)
    80002a50:	f04a                	sd	s2,32(sp)
    80002a52:	ec4e                	sd	s3,24(sp)
    80002a54:	e852                	sd	s4,16(sp)
    80002a56:	e456                	sd	s5,8(sp)
    80002a58:	8aaa                	mv	s5,a0
    80002a5a:	8a2e                	mv	s4,a1
    80002a5c:	89b2                	mv	s3,a2
  int i = 0;
    80002a5e:	4901                	li	s2,0
  while (i < count) { // count can be greater than page size
    80002a60:	a819                	j	80002a76 <sys_user_read+0x2e>
    uint64 addr = (uint64)bufva + i;
    uint64 pa = lookup_pa((pagetable_t)current->pagetable, addr);
    uint64 off = addr - ROUNDDOWN(addr, PGSIZE);
    uint64 len = count - i < PGSIZE - off ? count - i : PGSIZE - off;
    uint64 r = do_read(fd, (char *)pa + off, len);
    80002a62:	8622                	mv	a2,s0
    80002a64:	009505b3          	add	a1,a0,s1
    80002a68:	8556                	mv	a0,s5
    80002a6a:	857fe0ef          	jal	800012c0 <do_read>
    i += r; if (r < len) return i;
    80002a6e:	0125093b          	addw	s2,a0,s2
    80002a72:	02856963          	bltu	a0,s0,80002aa4 <sys_user_read+0x5c>
  while (i < count) { // count can be greater than page size
    80002a76:	03397963          	bgeu	s2,s3,80002aa8 <sys_user_read+0x60>
    uint64 addr = (uint64)bufva + i;
    80002a7a:	012a04b3          	add	s1,s4,s2
    uint64 pa = lookup_pa((pagetable_t)current->pagetable, addr);
    80002a7e:	85a6                	mv	a1,s1
    80002a80:	0000d797          	auipc	a5,0xd
    80002a84:	6a87b783          	ld	a5,1704(a5) # 80010128 <current>
    80002a88:	6788                	ld	a0,8(a5)
    80002a8a:	6eb000ef          	jal	80003974 <lookup_pa>
    uint64 off = addr - ROUNDDOWN(addr, PGSIZE);
    80002a8e:	6405                	lui	s0,0x1
    80002a90:	fff40793          	addi	a5,s0,-1 # fff <elf_fpread-0x7ffff001>
    80002a94:	8cfd                	and	s1,s1,a5
    uint64 len = count - i < PGSIZE - off ? count - i : PGSIZE - off;
    80002a96:	412987b3          	sub	a5,s3,s2
    80002a9a:	8c05                	sub	s0,s0,s1
    80002a9c:	fc87f3e3          	bgeu	a5,s0,80002a62 <sys_user_read+0x1a>
    80002aa0:	843e                	mv	s0,a5
    80002aa2:	b7c1                	j	80002a62 <sys_user_read+0x1a>
    i += r; if (r < len) return i;
    80002aa4:	854a                	mv	a0,s2
    80002aa6:	a011                	j	80002aaa <sys_user_read+0x62>
  }
  return count;
    80002aa8:	854e                	mv	a0,s3
}
    80002aaa:	70e2                	ld	ra,56(sp)
    80002aac:	7442                	ld	s0,48(sp)
    80002aae:	74a2                	ld	s1,40(sp)
    80002ab0:	7902                	ld	s2,32(sp)
    80002ab2:	69e2                	ld	s3,24(sp)
    80002ab4:	6a42                	ld	s4,16(sp)
    80002ab6:	6aa2                	ld	s5,8(sp)
    80002ab8:	6121                	addi	sp,sp,64
    80002aba:	8082                	ret

0000000080002abc <sys_user_write>:

//
// write file
//
ssize_t sys_user_write(int fd, char *bufva, uint64 count) {
    80002abc:	7139                	addi	sp,sp,-64
    80002abe:	fc06                	sd	ra,56(sp)
    80002ac0:	f822                	sd	s0,48(sp)
    80002ac2:	f426                	sd	s1,40(sp)
    80002ac4:	f04a                	sd	s2,32(sp)
    80002ac6:	ec4e                	sd	s3,24(sp)
    80002ac8:	e852                	sd	s4,16(sp)
    80002aca:	e456                	sd	s5,8(sp)
    80002acc:	8aaa                	mv	s5,a0
    80002ace:	8a2e                	mv	s4,a1
    80002ad0:	89b2                	mv	s3,a2
  int i = 0;
    80002ad2:	4901                	li	s2,0
  while (i < count) { // count can be greater than page size
    80002ad4:	a819                	j	80002aea <sys_user_write+0x2e>
    uint64 addr = (uint64)bufva + i;
    uint64 pa = lookup_pa((pagetable_t)current->pagetable, addr);
    uint64 off = addr - ROUNDDOWN(addr, PGSIZE);
    uint64 len = count - i < PGSIZE - off ? count - i : PGSIZE - off;
    uint64 r = do_write(fd, (char *)pa + off, len);
    80002ad6:	8622                	mv	a2,s0
    80002ad8:	009505b3          	add	a1,a0,s1
    80002adc:	8556                	mv	a0,s5
    80002ade:	841fe0ef          	jal	8000131e <do_write>
    i += r; if (r < len) return i;
    80002ae2:	0125093b          	addw	s2,a0,s2
    80002ae6:	02856963          	bltu	a0,s0,80002b18 <sys_user_write+0x5c>
  while (i < count) { // count can be greater than page size
    80002aea:	03397963          	bgeu	s2,s3,80002b1c <sys_user_write+0x60>
    uint64 addr = (uint64)bufva + i;
    80002aee:	012a04b3          	add	s1,s4,s2
    uint64 pa = lookup_pa((pagetable_t)current->pagetable, addr);
    80002af2:	85a6                	mv	a1,s1
    80002af4:	0000d797          	auipc	a5,0xd
    80002af8:	6347b783          	ld	a5,1588(a5) # 80010128 <current>
    80002afc:	6788                	ld	a0,8(a5)
    80002afe:	677000ef          	jal	80003974 <lookup_pa>
    uint64 off = addr - ROUNDDOWN(addr, PGSIZE);
    80002b02:	6405                	lui	s0,0x1
    80002b04:	fff40793          	addi	a5,s0,-1 # fff <elf_fpread-0x7ffff001>
    80002b08:	8cfd                	and	s1,s1,a5
    uint64 len = count - i < PGSIZE - off ? count - i : PGSIZE - off;
    80002b0a:	412987b3          	sub	a5,s3,s2
    80002b0e:	8c05                	sub	s0,s0,s1
    80002b10:	fc87f3e3          	bgeu	a5,s0,80002ad6 <sys_user_write+0x1a>
    80002b14:	843e                	mv	s0,a5
    80002b16:	b7c1                	j	80002ad6 <sys_user_write+0x1a>
    i += r; if (r < len) return i;
    80002b18:	854a                	mv	a0,s2
    80002b1a:	a011                	j	80002b1e <sys_user_write+0x62>
  }
  return count;
    80002b1c:	854e                	mv	a0,s3
}
    80002b1e:	70e2                	ld	ra,56(sp)
    80002b20:	7442                	ld	s0,48(sp)
    80002b22:	74a2                	ld	s1,40(sp)
    80002b24:	7902                	ld	s2,32(sp)
    80002b26:	69e2                	ld	s3,24(sp)
    80002b28:	6a42                	ld	s4,16(sp)
    80002b2a:	6aa2                	ld	s5,8(sp)
    80002b2c:	6121                	addi	sp,sp,64
    80002b2e:	8082                	ret

0000000080002b30 <sys_user_lseek>:

//
// lseek file
//
ssize_t sys_user_lseek(int fd, int offset, int whence) {
    80002b30:	1141                	addi	sp,sp,-16
    80002b32:	e406                	sd	ra,8(sp)
  return do_lseek(fd, offset, whence);
    80002b34:	81ffe0ef          	jal	80001352 <do_lseek>
}
    80002b38:	60a2                	ld	ra,8(sp)
    80002b3a:	0141                	addi	sp,sp,16
    80002b3c:	8082                	ret

0000000080002b3e <sys_user_stat>:

//
// read vinode
//
ssize_t sys_user_stat(int fd, struct istat *istat) {
    80002b3e:	1141                	addi	sp,sp,-16
    80002b40:	e406                	sd	ra,8(sp)
    80002b42:	e022                	sd	s0,0(sp)
    80002b44:	842a                	mv	s0,a0
  struct istat * pistat = (struct istat *)user_va_to_pa((pagetable_t)(current->pagetable), istat);
    80002b46:	0000d797          	auipc	a5,0xd
    80002b4a:	5e27b783          	ld	a5,1506(a5) # 80010128 <current>
    80002b4e:	6788                	ld	a0,8(a5)
    80002b50:	717000ef          	jal	80003a66 <user_va_to_pa>
    80002b54:	85aa                	mv	a1,a0
  return do_stat(fd, pistat);
    80002b56:	8522                	mv	a0,s0
    80002b58:	81ffe0ef          	jal	80001376 <do_stat>
}
    80002b5c:	60a2                	ld	ra,8(sp)
    80002b5e:	6402                	ld	s0,0(sp)
    80002b60:	0141                	addi	sp,sp,16
    80002b62:	8082                	ret

0000000080002b64 <sys_user_disk_stat>:

//
// read disk inode
//
ssize_t sys_user_disk_stat(int fd, struct istat *istat) {
    80002b64:	1141                	addi	sp,sp,-16
    80002b66:	e406                	sd	ra,8(sp)
    80002b68:	e022                	sd	s0,0(sp)
    80002b6a:	842a                	mv	s0,a0
  struct istat * pistat = (struct istat *)user_va_to_pa((pagetable_t)(current->pagetable), istat);
    80002b6c:	0000d797          	auipc	a5,0xd
    80002b70:	5bc7b783          	ld	a5,1468(a5) # 80010128 <current>
    80002b74:	6788                	ld	a0,8(a5)
    80002b76:	6f1000ef          	jal	80003a66 <user_va_to_pa>
    80002b7a:	85aa                	mv	a1,a0
  return do_disk_stat(fd, pistat);
    80002b7c:	8522                	mv	a0,s0
    80002b7e:	813fe0ef          	jal	80001390 <do_disk_stat>
}
    80002b82:	60a2                	ld	ra,8(sp)
    80002b84:	6402                	ld	s0,0(sp)
    80002b86:	0141                	addi	sp,sp,16
    80002b88:	8082                	ret

0000000080002b8a <sys_user_close>:

//
// close file
//
ssize_t sys_user_close(int fd) {
    80002b8a:	1141                	addi	sp,sp,-16
    80002b8c:	e406                	sd	ra,8(sp)
  return do_close(fd);
    80002b8e:	81dfe0ef          	jal	800013aa <do_close>
}
    80002b92:	60a2                	ld	ra,8(sp)
    80002b94:	0141                	addi	sp,sp,16
    80002b96:	8082                	ret

0000000080002b98 <sys_user_opendir>:

//
// lib call to opendir
//
ssize_t sys_user_opendir(char * pathva){
    80002b98:	1141                	addi	sp,sp,-16
    80002b9a:	e406                	sd	ra,8(sp)
    80002b9c:	85aa                	mv	a1,a0
  char * pathpa = (char*)user_va_to_pa((pagetable_t)(current->pagetable), pathva);
    80002b9e:	0000d797          	auipc	a5,0xd
    80002ba2:	58a7b783          	ld	a5,1418(a5) # 80010128 <current>
    80002ba6:	6788                	ld	a0,8(a5)
    80002ba8:	6bf000ef          	jal	80003a66 <user_va_to_pa>
  return do_opendir(pathpa);
    80002bac:	811fe0ef          	jal	800013bc <do_opendir>
}
    80002bb0:	60a2                	ld	ra,8(sp)
    80002bb2:	0141                	addi	sp,sp,16
    80002bb4:	8082                	ret

0000000080002bb6 <sys_user_readdir>:

//
// lib call to readdir
//
ssize_t sys_user_readdir(int fd, struct dir *vdir){
    80002bb6:	1141                	addi	sp,sp,-16
    80002bb8:	e406                	sd	ra,8(sp)
    80002bba:	e022                	sd	s0,0(sp)
    80002bbc:	842a                	mv	s0,a0
  struct dir * pdir = (struct dir *)user_va_to_pa((pagetable_t)(current->pagetable), vdir);
    80002bbe:	0000d797          	auipc	a5,0xd
    80002bc2:	56a7b783          	ld	a5,1386(a5) # 80010128 <current>
    80002bc6:	6788                	ld	a0,8(a5)
    80002bc8:	69f000ef          	jal	80003a66 <user_va_to_pa>
    80002bcc:	85aa                	mv	a1,a0
  return do_readdir(fd, pdir);
    80002bce:	8522                	mv	a0,s0
    80002bd0:	86bfe0ef          	jal	8000143a <do_readdir>
}
    80002bd4:	60a2                	ld	ra,8(sp)
    80002bd6:	6402                	ld	s0,0(sp)
    80002bd8:	0141                	addi	sp,sp,16
    80002bda:	8082                	ret

0000000080002bdc <sys_user_mkdir>:

//
// lib call to mkdir
//
ssize_t sys_user_mkdir(char * pathva){
    80002bdc:	1141                	addi	sp,sp,-16
    80002bde:	e406                	sd	ra,8(sp)
    80002be0:	85aa                	mv	a1,a0
  char * pathpa = (char*)user_va_to_pa((pagetable_t)(current->pagetable), pathva);
    80002be2:	0000d797          	auipc	a5,0xd
    80002be6:	5467b783          	ld	a5,1350(a5) # 80010128 <current>
    80002bea:	6788                	ld	a0,8(a5)
    80002bec:	67b000ef          	jal	80003a66 <user_va_to_pa>
  return do_mkdir(pathpa);
    80002bf0:	865fe0ef          	jal	80001454 <do_mkdir>
}
    80002bf4:	60a2                	ld	ra,8(sp)
    80002bf6:	0141                	addi	sp,sp,16
    80002bf8:	8082                	ret

0000000080002bfa <sys_user_closedir>:

//
// lib call to closedir
//
ssize_t sys_user_closedir(int fd){
    80002bfa:	1141                	addi	sp,sp,-16
    80002bfc:	e406                	sd	ra,8(sp)
  return do_closedir(fd);
    80002bfe:	865fe0ef          	jal	80001462 <do_closedir>
}
    80002c02:	60a2                	ld	ra,8(sp)
    80002c04:	0141                	addi	sp,sp,16
    80002c06:	8082                	ret

0000000080002c08 <sys_user_link>:

//
// lib call to link
//
ssize_t sys_user_link(char * vfn1, char * vfn2){
    80002c08:	1101                	addi	sp,sp,-32
    80002c0a:	ec06                	sd	ra,24(sp)
    80002c0c:	e822                	sd	s0,16(sp)
    80002c0e:	e426                	sd	s1,8(sp)
    80002c10:	e04a                	sd	s2,0(sp)
    80002c12:	84ae                	mv	s1,a1
  char * pfn1 = (char*)user_va_to_pa((pagetable_t)(current->pagetable), (void*)vfn1);
    80002c14:	0000d917          	auipc	s2,0xd
    80002c18:	51490913          	addi	s2,s2,1300 # 80010128 <current>
    80002c1c:	00093783          	ld	a5,0(s2)
    80002c20:	85aa                	mv	a1,a0
    80002c22:	6788                	ld	a0,8(a5)
    80002c24:	643000ef          	jal	80003a66 <user_va_to_pa>
    80002c28:	842a                	mv	s0,a0
  char * pfn2 = (char*)user_va_to_pa((pagetable_t)(current->pagetable), (void*)vfn2);
    80002c2a:	00093783          	ld	a5,0(s2)
    80002c2e:	85a6                	mv	a1,s1
    80002c30:	6788                	ld	a0,8(a5)
    80002c32:	635000ef          	jal	80003a66 <user_va_to_pa>
    80002c36:	85aa                	mv	a1,a0
  return do_link(pfn1, pfn2);
    80002c38:	8522                	mv	a0,s0
    80002c3a:	83bfe0ef          	jal	80001474 <do_link>
}
    80002c3e:	60e2                	ld	ra,24(sp)
    80002c40:	6442                	ld	s0,16(sp)
    80002c42:	64a2                	ld	s1,8(sp)
    80002c44:	6902                	ld	s2,0(sp)
    80002c46:	6105                	addi	sp,sp,32
    80002c48:	8082                	ret

0000000080002c4a <sys_user_unlink>:

//
// lib call to unlink
//
ssize_t sys_user_unlink(char * vfn){
    80002c4a:	1141                	addi	sp,sp,-16
    80002c4c:	e406                	sd	ra,8(sp)
    80002c4e:	85aa                	mv	a1,a0
  char * pfn = (char*)user_va_to_pa((pagetable_t)(current->pagetable), (void*)vfn);
    80002c50:	0000d797          	auipc	a5,0xd
    80002c54:	4d87b783          	ld	a5,1240(a5) # 80010128 <current>
    80002c58:	6788                	ld	a0,8(a5)
    80002c5a:	60d000ef          	jal	80003a66 <user_va_to_pa>
  return do_unlink(pfn);
    80002c5e:	825fe0ef          	jal	80001482 <do_unlink>
}
    80002c62:	60a2                	ld	ra,8(sp)
    80002c64:	0141                	addi	sp,sp,16
    80002c66:	8082                	ret

0000000080002c68 <sys_user_exec>:

ssize_t sys_user_exec(char *vfn) {
    80002c68:	1141                	addi	sp,sp,-16
    80002c6a:	e406                	sd	ra,8(sp)
    80002c6c:	85aa                	mv	a1,a0
  char *pfn = (char*)user_va_to_pa((pagetable_t)(current->pagetable), (void*)vfn);
    80002c6e:	0000d797          	auipc	a5,0xd
    80002c72:	4ba7b783          	ld	a5,1210(a5) # 80010128 <current>
    80002c76:	6788                	ld	a0,8(a5)
    80002c78:	5ef000ef          	jal	80003a66 <user_va_to_pa>
  return do_exec(pfn);
    80002c7c:	815fe0ef          	jal	80001490 <do_exec>
}
    80002c80:	60a2                	ld	ra,8(sp)
    80002c82:	0141                	addi	sp,sp,16
    80002c84:	8082                	ret

0000000080002c86 <do_syscall>:

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7) {
    80002c86:	1141                	addi	sp,sp,-16
    80002c88:	e406                	sd	ra,8(sp)
    80002c8a:	872a                	mv	a4,a0
    80002c8c:	852e                	mv	a0,a1
    80002c8e:	85b2                	mv	a1,a2
  switch (a0) {
    80002c90:	fc070793          	addi	a5,a4,-64
    80002c94:	4679                	li	a2,30
    80002c96:	0af66763          	bltu	a2,a5,80002d44 <do_syscall+0xbe>
    80002c9a:	078a                	slli	a5,a5,0x2
    80002c9c:	00004617          	auipc	a2,0x4
    80002ca0:	45460613          	addi	a2,a2,1108 # 800070f0 <rfs_i_ops+0x78>
    80002ca4:	97b2                	add	a5,a5,a2
    80002ca6:	439c                	lw	a5,0(a5)
    80002ca8:	97b2                	add	a5,a5,a2
    80002caa:	8782                	jr	a5
    case SYS_user_print:
      return sys_user_print((const char*)a1, a2);
    80002cac:	be3ff0ef          	jal	8000288e <sys_user_print>
     case SYS_user_exec:
      return sys_user_exec((char *)a1);
    default:
      panic("Unknown syscall %ld \n", a0);
  }
}
    80002cb0:	60a2                	ld	ra,8(sp)
    80002cb2:	0141                	addi	sp,sp,16
    80002cb4:	8082                	ret
      return sys_user_exit(a1);
    80002cb6:	c21ff0ef          	jal	800028d6 <sys_user_exit>
    80002cba:	bfdd                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_allocate_page();
    80002cbc:	c45ff0ef          	jal	80002900 <sys_user_allocate_page>
    80002cc0:	bfc5                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_free_page(a1);
    80002cc2:	cd7ff0ef          	jal	80002998 <sys_user_free_page>
    80002cc6:	b7ed                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_fork();
    80002cc8:	d17ff0ef          	jal	800029de <sys_user_fork>
    80002ccc:	b7d5                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_yield();
    80002cce:	d33ff0ef          	jal	80002a00 <sys_user_yield>
    80002cd2:	bff9                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_open((char *)a1, a2);
    80002cd4:	2581                	sext.w	a1,a1
    80002cd6:	d4dff0ef          	jal	80002a22 <sys_user_open>
    80002cda:	bfd9                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_read(a1, (char *)a2, a3);
    80002cdc:	8636                	mv	a2,a3
    80002cde:	2501                	sext.w	a0,a0
    80002ce0:	d69ff0ef          	jal	80002a48 <sys_user_read>
    80002ce4:	b7f1                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_write(a1, (char *)a2, a3);
    80002ce6:	8636                	mv	a2,a3
    80002ce8:	2501                	sext.w	a0,a0
    80002cea:	dd3ff0ef          	jal	80002abc <sys_user_write>
    80002cee:	b7c9                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_lseek(a1, a2, a3);
    80002cf0:	0006861b          	sext.w	a2,a3
    80002cf4:	2581                	sext.w	a1,a1
    80002cf6:	2501                	sext.w	a0,a0
    80002cf8:	e39ff0ef          	jal	80002b30 <sys_user_lseek>
    80002cfc:	bf55                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_stat(a1, (struct istat *)a2);
    80002cfe:	2501                	sext.w	a0,a0
    80002d00:	e3fff0ef          	jal	80002b3e <sys_user_stat>
    80002d04:	b775                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_disk_stat(a1, (struct istat *)a2);
    80002d06:	2501                	sext.w	a0,a0
    80002d08:	e5dff0ef          	jal	80002b64 <sys_user_disk_stat>
    80002d0c:	b755                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_close(a1);
    80002d0e:	2501                	sext.w	a0,a0
    80002d10:	e7bff0ef          	jal	80002b8a <sys_user_close>
    80002d14:	bf71                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_opendir((char *)a1);
    80002d16:	e83ff0ef          	jal	80002b98 <sys_user_opendir>
    80002d1a:	bf59                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_readdir(a1, (struct dir *)a2);
    80002d1c:	2501                	sext.w	a0,a0
    80002d1e:	e99ff0ef          	jal	80002bb6 <sys_user_readdir>
    80002d22:	b779                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_mkdir((char *)a1);
    80002d24:	eb9ff0ef          	jal	80002bdc <sys_user_mkdir>
    80002d28:	b761                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_closedir(a1);
    80002d2a:	2501                	sext.w	a0,a0
    80002d2c:	ecfff0ef          	jal	80002bfa <sys_user_closedir>
    80002d30:	b741                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_link((char *)a1, (char *)a2);
    80002d32:	ed7ff0ef          	jal	80002c08 <sys_user_link>
    80002d36:	bfad                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_unlink((char *)a1);
    80002d38:	f13ff0ef          	jal	80002c4a <sys_user_unlink>
    80002d3c:	bf95                	j	80002cb0 <do_syscall+0x2a>
      return sys_user_exec((char *)a1);
    80002d3e:	f2bff0ef          	jal	80002c68 <sys_user_exec>
    80002d42:	b7bd                	j	80002cb0 <do_syscall+0x2a>
      panic("Unknown syscall %ld \n", a0);
    80002d44:	85ba                	mv	a1,a4
    80002d46:	00005517          	auipc	a0,0x5
    80002d4a:	62a50513          	addi	a0,a0,1578 # 80008370 <rfs_i_ops+0x12f8>
    80002d4e:	749010ef          	jal	80004c96 <do_panic>

0000000080002d52 <dentry_hash_func>:
  return 0;
}

size_t dentry_hash_func(void *key) {
  struct dentry_key *dentry_key = key;
  char *name = dentry_key->name;
    80002d52:	6518                	ld	a4,8(a0)

  size_t hash = 5381;
    80002d54:	6785                	lui	a5,0x1
    80002d56:	50578793          	addi	a5,a5,1285 # 1505 <elf_fpread-0x7fffeafb>
  int c;

  while ((c = *name++)) hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    80002d5a:	a031                	j	80002d66 <dentry_hash_func+0x14>
    80002d5c:	00579613          	slli	a2,a5,0x5
    80002d60:	97b2                	add	a5,a5,a2
    80002d62:	97ba                	add	a5,a5,a4
    80002d64:	8736                	mv	a4,a3
    80002d66:	00170693          	addi	a3,a4,1
    80002d6a:	00074703          	lbu	a4,0(a4)
    80002d6e:	f77d                	bnez	a4,80002d5c <dentry_hash_func+0xa>

  hash = ((hash << 5) + hash) + (size_t)dentry_key->parent;
    80002d70:	00579713          	slli	a4,a5,0x5
    80002d74:	97ba                	add	a5,a5,a4
    80002d76:	6108                	ld	a0,0(a0)
    80002d78:	953e                	add	a0,a0,a5
  return hash % HASH_TABLE_SIZE;
}
    80002d7a:	07f57513          	andi	a0,a0,127
    80002d7e:	8082                	ret

0000000080002d80 <vinode_hash_equal>:

// vinode generic hash table method implementation
int vinode_hash_equal(void *key1, void *key2) {
  struct vinode_key *vinode_key1 = key1;
  struct vinode_key *vinode_key2 = key2;
  if (vinode_key1->inum == vinode_key2->inum && vinode_key1->sb == vinode_key2->sb) {
    80002d80:	4118                	lw	a4,0(a0)
    80002d82:	419c                	lw	a5,0(a1)
    80002d84:	00f70463          	beq	a4,a5,80002d8c <vinode_hash_equal+0xc>
    return 1;
  }
  return 0;
    80002d88:	4501                	li	a0,0
    80002d8a:	8082                	ret
  if (vinode_key1->inum == vinode_key2->inum && vinode_key1->sb == vinode_key2->sb) {
    80002d8c:	6518                	ld	a4,8(a0)
    80002d8e:	659c                	ld	a5,8(a1)
    80002d90:	00f70463          	beq	a4,a5,80002d98 <vinode_hash_equal+0x18>
  return 0;
    80002d94:	4501                	li	a0,0
    80002d96:	8082                	ret
    return 1;
    80002d98:	4505                	li	a0,1
}
    80002d9a:	8082                	ret

0000000080002d9c <vinode_hash_func>:

size_t vinode_hash_func(void *key) {
  struct vinode_key *vinode_key = key;
  return vinode_key->inum % HASH_TABLE_SIZE;
    80002d9c:	411c                	lw	a5,0(a0)
    80002d9e:	41f7d51b          	sraiw	a0,a5,0x1f
    80002da2:	0195551b          	srliw	a0,a0,0x19
    80002da6:	9fa9                	addw	a5,a5,a0
    80002da8:	07f7f793          	andi	a5,a5,127
}
    80002dac:	40a7853b          	subw	a0,a5,a0
    80002db0:	8082                	ret

0000000080002db2 <dentry_hash_equal>:
int dentry_hash_equal(void *key1, void *key2) {
    80002db2:	1101                	addi	sp,sp,-32
    80002db4:	ec06                	sd	ra,24(sp)
    80002db6:	e822                	sd	s0,16(sp)
    80002db8:	e426                	sd	s1,8(sp)
    80002dba:	84aa                	mv	s1,a0
    80002dbc:	842e                	mv	s0,a1
  if (strcmp(dentry_key1->name, dentry_key2->name) == 0 &&
    80002dbe:	658c                	ld	a1,8(a1)
    80002dc0:	6508                	ld	a0,8(a0)
    80002dc2:	3e8010ef          	jal	800041aa <strcmp>
    80002dc6:	e519                	bnez	a0,80002dd4 <dentry_hash_equal+0x22>
      dentry_key1->parent == dentry_key2->parent) {
    80002dc8:	6098                	ld	a4,0(s1)
    80002dca:	601c                	ld	a5,0(s0)
  if (strcmp(dentry_key1->name, dentry_key2->name) == 0 &&
    80002dcc:	00f71563          	bne	a4,a5,80002dd6 <dentry_hash_equal+0x24>
    return 1;
    80002dd0:	4505                	li	a0,1
    80002dd2:	a011                	j	80002dd6 <dentry_hash_equal+0x24>
  return 0;
    80002dd4:	4501                	li	a0,0
}
    80002dd6:	60e2                	ld	ra,24(sp)
    80002dd8:	6442                	ld	s0,16(sp)
    80002dda:	64a2                	ld	s1,8(sp)
    80002ddc:	6105                	addi	sp,sp,32
    80002dde:	8082                	ret

0000000080002de0 <vfs_init>:
int vfs_init() {
    80002de0:	1141                	addi	sp,sp,-16
    80002de2:	e406                	sd	ra,8(sp)
  ret = hash_table_init(&dentry_hash_table, dentry_hash_equal, dentry_hash_func,
    80002de4:	4781                	li	a5,0
    80002de6:	4701                	li	a4,0
    80002de8:	4681                	li	a3,0
    80002dea:	00000617          	auipc	a2,0x0
    80002dee:	f6860613          	addi	a2,a2,-152 # 80002d52 <dentry_hash_func>
    80002df2:	00000597          	auipc	a1,0x0
    80002df6:	fc058593          	addi	a1,a1,-64 # 80002db2 <dentry_hash_equal>
    80002dfa:	0000b517          	auipc	a0,0xb
    80002dfe:	17e50513          	addi	a0,a0,382 # 8000df78 <dentry_hash_table>
    80002e02:	29c010ef          	jal	8000409e <hash_table_init>
  if (ret != 0) return ret;
    80002e06:	c501                	beqz	a0,80002e0e <vfs_init+0x2e>
}
    80002e08:	60a2                	ld	ra,8(sp)
    80002e0a:	0141                	addi	sp,sp,16
    80002e0c:	8082                	ret
  ret = hash_table_init(&vinode_hash_table, vinode_hash_equal, vinode_hash_func,
    80002e0e:	4781                	li	a5,0
    80002e10:	4701                	li	a4,0
    80002e12:	4681                	li	a3,0
    80002e14:	00000617          	auipc	a2,0x0
    80002e18:	f8860613          	addi	a2,a2,-120 # 80002d9c <vinode_hash_func>
    80002e1c:	00000597          	auipc	a1,0x0
    80002e20:	f6458593          	addi	a1,a1,-156 # 80002d80 <vinode_hash_equal>
    80002e24:	0000a517          	auipc	a0,0xa
    80002e28:	52c50513          	addi	a0,a0,1324 # 8000d350 <vinode_hash_table>
    80002e2c:	272010ef          	jal	8000409e <hash_table_init>
  if (ret != 0) return ret;
    80002e30:	bfe1                	j	80002e08 <vfs_init+0x28>

0000000080002e32 <vfs_read>:
ssize_t vfs_read(struct file *file, char *buf, size_t count) {
    80002e32:	1141                	addi	sp,sp,-16
    80002e34:	e406                	sd	ra,8(sp)
  if (!file->readable) {
    80002e36:	415c                	lw	a5,4(a0)
    80002e38:	cf89                	beqz	a5,80002e52 <vfs_read+0x20>
    80002e3a:	86aa                	mv	a3,a0
  if (file->f_dentry->dentry_inode->type != FILE_I) {
    80002e3c:	691c                	ld	a5,16(a0)
    80002e3e:	7788                	ld	a0,40(a5)
    80002e40:	455c                	lw	a5,12(a0)
    80002e42:	e385                	bnez	a5,80002e62 <vfs_read+0x30>
  return viop_read(file->f_dentry->dentry_inode, buf, count, &(file->offset));
    80002e44:	693c                	ld	a5,80(a0)
    80002e46:	639c                	ld	a5,0(a5)
    80002e48:	06b1                	addi	a3,a3,12
    80002e4a:	9782                	jalr	a5
}
    80002e4c:	60a2                	ld	ra,8(sp)
    80002e4e:	0141                	addi	sp,sp,16
    80002e50:	8082                	ret
    sprint("vfs_read: file is not readable!\n");
    80002e52:	00005517          	auipc	a0,0x5
    80002e56:	53650513          	addi	a0,a0,1334 # 80008388 <rfs_i_ops+0x1310>
    80002e5a:	599010ef          	jal	80004bf2 <sprint>
    return -1;
    80002e5e:	557d                	li	a0,-1
    80002e60:	b7f5                	j	80002e4c <vfs_read+0x1a>
    sprint("vfs_read: cannot read a directory!\n");
    80002e62:	00005517          	auipc	a0,0x5
    80002e66:	54e50513          	addi	a0,a0,1358 # 800083b0 <rfs_i_ops+0x1338>
    80002e6a:	589010ef          	jal	80004bf2 <sprint>
    return -1;
    80002e6e:	557d                	li	a0,-1
    80002e70:	bff1                	j	80002e4c <vfs_read+0x1a>

0000000080002e72 <vfs_write>:
ssize_t vfs_write(struct file *file, const char *buf, size_t count) {
    80002e72:	1141                	addi	sp,sp,-16
    80002e74:	e406                	sd	ra,8(sp)
  if (!file->writable) {
    80002e76:	451c                	lw	a5,8(a0)
    80002e78:	cf89                	beqz	a5,80002e92 <vfs_write+0x20>
    80002e7a:	86aa                	mv	a3,a0
  if (file->f_dentry->dentry_inode->type != FILE_I) {
    80002e7c:	691c                	ld	a5,16(a0)
    80002e7e:	7788                	ld	a0,40(a5)
    80002e80:	455c                	lw	a5,12(a0)
    80002e82:	e385                	bnez	a5,80002ea2 <vfs_write+0x30>
  return viop_write(file->f_dentry->dentry_inode, buf, count, &(file->offset));
    80002e84:	693c                	ld	a5,80(a0)
    80002e86:	679c                	ld	a5,8(a5)
    80002e88:	06b1                	addi	a3,a3,12
    80002e8a:	9782                	jalr	a5
}
    80002e8c:	60a2                	ld	ra,8(sp)
    80002e8e:	0141                	addi	sp,sp,16
    80002e90:	8082                	ret
    sprint("vfs_write: file is not writable!\n");
    80002e92:	00005517          	auipc	a0,0x5
    80002e96:	54650513          	addi	a0,a0,1350 # 800083d8 <rfs_i_ops+0x1360>
    80002e9a:	559010ef          	jal	80004bf2 <sprint>
    return -1;
    80002e9e:	557d                	li	a0,-1
    80002ea0:	b7f5                	j	80002e8c <vfs_write+0x1a>
    sprint("vfs_read: cannot write a directory!\n");
    80002ea2:	00005517          	auipc	a0,0x5
    80002ea6:	55e50513          	addi	a0,a0,1374 # 80008400 <rfs_i_ops+0x1388>
    80002eaa:	549010ef          	jal	80004bf2 <sprint>
    return -1;
    80002eae:	557d                	li	a0,-1
    80002eb0:	bff1                	j	80002e8c <vfs_write+0x1a>

0000000080002eb2 <vfs_lseek>:
ssize_t vfs_lseek(struct file *file, ssize_t offset, int whence) {
    80002eb2:	1141                	addi	sp,sp,-16
    80002eb4:	e406                	sd	ra,8(sp)
    80002eb6:	e022                	sd	s0,0(sp)
    80002eb8:	842a                	mv	s0,a0
  if (file->f_dentry->dentry_inode->type != FILE_I) {
    80002eba:	691c                	ld	a5,16(a0)
    80002ebc:	7788                	ld	a0,40(a5)
    80002ebe:	455c                	lw	a5,12(a0)
    80002ec0:	ef81                	bnez	a5,80002ed8 <vfs_lseek+0x26>
  if (viop_lseek(file->f_dentry->dentry_inode, offset, whence, &(file->offset)) != 0) {
    80002ec2:	693c                	ld	a5,80(a0)
    80002ec4:	6f9c                	ld	a5,24(a5)
    80002ec6:	00c40693          	addi	a3,s0,12
    80002eca:	9782                	jalr	a5
    80002ecc:	ed11                	bnez	a0,80002ee8 <vfs_lseek+0x36>
  return file->offset;
    80002ece:	4448                	lw	a0,12(s0)
}
    80002ed0:	60a2                	ld	ra,8(sp)
    80002ed2:	6402                	ld	s0,0(sp)
    80002ed4:	0141                	addi	sp,sp,16
    80002ed6:	8082                	ret
    sprint("vfs_read: cannot seek a directory!\n");
    80002ed8:	00005517          	auipc	a0,0x5
    80002edc:	55050513          	addi	a0,a0,1360 # 80008428 <rfs_i_ops+0x13b0>
    80002ee0:	513010ef          	jal	80004bf2 <sprint>
    return -1;
    80002ee4:	557d                	li	a0,-1
    80002ee6:	b7ed                	j	80002ed0 <vfs_lseek+0x1e>
    sprint("vfs_lseek: lseek failed!\n");
    80002ee8:	00005517          	auipc	a0,0x5
    80002eec:	56850513          	addi	a0,a0,1384 # 80008450 <rfs_i_ops+0x13d8>
    80002ef0:	503010ef          	jal	80004bf2 <sprint>
    return -1;
    80002ef4:	557d                	li	a0,-1
    80002ef6:	bfe9                	j	80002ed0 <vfs_lseek+0x1e>

0000000080002ef8 <vfs_stat>:
  istat->st_inum = file->f_dentry->dentry_inode->inum;
    80002ef8:	691c                	ld	a5,16(a0)
    80002efa:	779c                	ld	a5,40(a5)
    80002efc:	439c                	lw	a5,0(a5)
    80002efe:	c19c                	sw	a5,0(a1)
  istat->st_size = file->f_dentry->dentry_inode->size;
    80002f00:	691c                	ld	a5,16(a0)
    80002f02:	779c                	ld	a5,40(a5)
    80002f04:	479c                	lw	a5,8(a5)
    80002f06:	c1dc                	sw	a5,4(a1)
  istat->st_type = file->f_dentry->dentry_inode->type;
    80002f08:	691c                	ld	a5,16(a0)
    80002f0a:	779c                	ld	a5,40(a5)
    80002f0c:	47dc                	lw	a5,12(a5)
    80002f0e:	c59c                	sw	a5,8(a1)
  istat->st_nlinks = file->f_dentry->dentry_inode->nlinks;
    80002f10:	691c                	ld	a5,16(a0)
    80002f12:	779c                	ld	a5,40(a5)
    80002f14:	4b9c                	lw	a5,16(a5)
    80002f16:	c5dc                	sw	a5,12(a1)
  istat->st_blocks = file->f_dentry->dentry_inode->blocks;
    80002f18:	691c                	ld	a5,16(a0)
    80002f1a:	779c                	ld	a5,40(a5)
    80002f1c:	4bdc                	lw	a5,20(a5)
    80002f1e:	c99c                	sw	a5,16(a1)
}
    80002f20:	4501                	li	a0,0
    80002f22:	8082                	ret

0000000080002f24 <vfs_disk_stat>:
int vfs_disk_stat(struct file *file, struct istat *istat) {
    80002f24:	1141                	addi	sp,sp,-16
    80002f26:	e406                	sd	ra,8(sp)
  return viop_disk_stat(file->f_dentry->dentry_inode, istat);
    80002f28:	691c                	ld	a5,16(a0)
    80002f2a:	7788                	ld	a0,40(a5)
    80002f2c:	693c                	ld	a5,80(a0)
    80002f2e:	739c                	ld	a5,32(a5)
    80002f30:	9782                	jalr	a5
}
    80002f32:	60a2                	ld	ra,8(sp)
    80002f34:	0141                	addi	sp,sp,16
    80002f36:	8082                	ret

0000000080002f38 <vfs_readdir>:
int vfs_readdir(struct file *file, struct dir *dir) {
    80002f38:	1141                	addi	sp,sp,-16
    80002f3a:	e406                	sd	ra,8(sp)
    80002f3c:	862a                	mv	a2,a0
  if (file->f_dentry->dentry_inode->type != DIR_I) {
    80002f3e:	691c                	ld	a5,16(a0)
    80002f40:	7788                	ld	a0,40(a5)
    80002f42:	4558                	lw	a4,12(a0)
    80002f44:	4785                	li	a5,1
    80002f46:	00f71963          	bne	a4,a5,80002f58 <vfs_readdir+0x20>
  return viop_readdir(file->f_dentry->dentry_inode, dir, &(file->offset));
    80002f4a:	693c                	ld	a5,80(a0)
    80002f4c:	63bc                	ld	a5,64(a5)
    80002f4e:	0631                	addi	a2,a2,12
    80002f50:	9782                	jalr	a5
}
    80002f52:	60a2                	ld	ra,8(sp)
    80002f54:	0141                	addi	sp,sp,16
    80002f56:	8082                	ret
    sprint("vfs_readdir: cannot read a file!\n");
    80002f58:	00005517          	auipc	a0,0x5
    80002f5c:	51850513          	addi	a0,a0,1304 # 80008470 <rfs_i_ops+0x13f8>
    80002f60:	493010ef          	jal	80004bf2 <sprint>
    return -1;
    80002f64:	557d                	li	a0,-1
    80002f66:	b7f5                	j	80002f52 <vfs_readdir+0x1a>

0000000080002f68 <vfs_closedir>:
int vfs_closedir(struct file *file) {
    80002f68:	1141                	addi	sp,sp,-16
    80002f6a:	e406                	sd	ra,8(sp)
  if (file->f_dentry->dentry_inode->type != DIR_I) {
    80002f6c:	691c                	ld	a5,16(a0)
    80002f6e:	7798                	ld	a4,40(a5)
    80002f70:	4754                	lw	a3,12(a4)
    80002f72:	4705                	li	a4,1
    80002f74:	02e69163          	bne	a3,a4,80002f96 <vfs_closedir+0x2e>
  file->f_dentry->d_ref--;
    80002f78:	5398                	lw	a4,32(a5)
    80002f7a:	377d                	addiw	a4,a4,-1
    80002f7c:	d398                	sw	a4,32(a5)
  file->status = FD_NONE;
    80002f7e:	00052023          	sw	zero,0(a0)
  if (file->f_dentry->dentry_inode->i_ops->viop_hook_closedir) {
    80002f82:	690c                	ld	a1,16(a0)
    80002f84:	7588                	ld	a0,40(a1)
    80002f86:	693c                	ld	a5,80(a0)
    80002f88:	7bbc                	ld	a5,112(a5)
    80002f8a:	c795                	beqz	a5,80002fb6 <vfs_closedir+0x4e>
    if (file->f_dentry->dentry_inode->i_ops->
    80002f8c:	9782                	jalr	a5
    80002f8e:	ed01                	bnez	a0,80002fa6 <vfs_closedir+0x3e>
}
    80002f90:	60a2                	ld	ra,8(sp)
    80002f92:	0141                	addi	sp,sp,16
    80002f94:	8082                	ret
    sprint("vfs_closedir: cannot close a file!\n");
    80002f96:	00005517          	auipc	a0,0x5
    80002f9a:	50250513          	addi	a0,a0,1282 # 80008498 <rfs_i_ops+0x1420>
    80002f9e:	455010ef          	jal	80004bf2 <sprint>
    return -1;
    80002fa2:	557d                	li	a0,-1
    80002fa4:	b7f5                	j	80002f90 <vfs_closedir+0x28>
      sprint("vfs_closedir: hook closedir failed!\n");
    80002fa6:	00005517          	auipc	a0,0x5
    80002faa:	51a50513          	addi	a0,a0,1306 # 800084c0 <rfs_i_ops+0x1448>
    80002fae:	445010ef          	jal	80004bf2 <sprint>
  return 0;
    80002fb2:	4501                	li	a0,0
    80002fb4:	bff1                	j	80002f90 <vfs_closedir+0x28>
    80002fb6:	4501                	li	a0,0
    80002fb8:	bfe1                	j	80002f90 <vfs_closedir+0x28>

0000000080002fba <get_base_name>:
void get_base_name(const char *path, char *base_name) {
    80002fba:	7139                	addi	sp,sp,-64
    80002fbc:	fc06                	sd	ra,56(sp)
    80002fbe:	f822                	sd	s0,48(sp)
    80002fc0:	f426                	sd	s1,40(sp)
    80002fc2:	84ae                	mv	s1,a1
  strcpy(path_copy, path);
    80002fc4:	85aa                	mv	a1,a0
    80002fc6:	850a                	mv	a0,sp
    80002fc8:	1fa010ef          	jal	800041c2 <strcpy>
  char *token = strtok(path_copy, "/");
    80002fcc:	00004597          	auipc	a1,0x4
    80002fd0:	4cc58593          	addi	a1,a1,1228 # 80007498 <rfs_i_ops+0x420>
    80002fd4:	850a                	mv	a0,sp
    80002fd6:	214010ef          	jal	800041ea <strtok>
    80002fda:	842a                	mv	s0,a0
  char *last_token = NULL;
    80002fdc:	4581                	li	a1,0
  while (token != NULL) {
    80002fde:	a811                	j	80002ff2 <get_base_name+0x38>
    token = strtok(NULL, "/");
    80002fe0:	00004597          	auipc	a1,0x4
    80002fe4:	4b858593          	addi	a1,a1,1208 # 80007498 <rfs_i_ops+0x420>
    80002fe8:	4501                	li	a0,0
    80002fea:	200010ef          	jal	800041ea <strtok>
    last_token = token;
    80002fee:	85a2                	mv	a1,s0
    token = strtok(NULL, "/");
    80002ff0:	842a                	mv	s0,a0
  while (token != NULL) {
    80002ff2:	f47d                	bnez	s0,80002fe0 <get_base_name+0x26>
  strcpy(base_name, last_token);
    80002ff4:	8526                	mv	a0,s1
    80002ff6:	1cc010ef          	jal	800041c2 <strcpy>
}
    80002ffa:	70e2                	ld	ra,56(sp)
    80002ffc:	7442                	ld	s0,48(sp)
    80002ffe:	74a2                	ld	s1,40(sp)
    80003000:	6121                	addi	sp,sp,64
    80003002:	8082                	ret

0000000080003004 <alloc_vfs_file>:
                        int offset) {
    80003004:	1101                	addi	sp,sp,-32
    80003006:	ec06                	sd	ra,24(sp)
    80003008:	e822                	sd	s0,16(sp)
    8000300a:	e426                	sd	s1,8(sp)
    8000300c:	e04a                	sd	s2,0(sp)
    8000300e:	842a                	mv	s0,a0
    80003010:	892e                	mv	s2,a1
    80003012:	84b2                	mv	s1,a2
  struct file *file = alloc_page();
    80003014:	fc8fd0ef          	jal	800007dc <alloc_page>
  file->f_dentry = file_dentry;
    80003018:	e900                	sd	s0,16(a0)
  file_dentry->d_ref += 1;
    8000301a:	5018                	lw	a4,32(s0)
    8000301c:	2705                	addiw	a4,a4,1
    8000301e:	d018                	sw	a4,32(s0)
  file->readable = readable;
    80003020:	01252223          	sw	s2,4(a0)
  file->writable = writable;
    80003024:	c504                	sw	s1,8(a0)
  file->offset = 0;
    80003026:	00052623          	sw	zero,12(a0)
  file->status = FD_OPENED;
    8000302a:	4705                	li	a4,1
    8000302c:	c118                	sw	a4,0(a0)
}
    8000302e:	60e2                	ld	ra,24(sp)
    80003030:	6442                	ld	s0,16(sp)
    80003032:	64a2                	ld	s1,8(sp)
    80003034:	6902                	ld	s2,0(sp)
    80003036:	6105                	addi	sp,sp,32
    80003038:	8082                	ret

000000008000303a <alloc_vfs_dentry>:
                            struct dentry *parent) {
    8000303a:	7179                	addi	sp,sp,-48
    8000303c:	f406                	sd	ra,40(sp)
    8000303e:	f022                	sd	s0,32(sp)
    80003040:	ec26                	sd	s1,24(sp)
    80003042:	e84a                	sd	s2,16(sp)
    80003044:	e44e                	sd	s3,8(sp)
    80003046:	89aa                	mv	s3,a0
    80003048:	84ae                	mv	s1,a1
    8000304a:	8932                	mv	s2,a2
  struct dentry *dentry = (struct dentry *)alloc_page();
    8000304c:	f90fd0ef          	jal	800007dc <alloc_page>
    80003050:	842a                	mv	s0,a0
  strcpy(dentry->name, name);
    80003052:	85ce                	mv	a1,s3
    80003054:	16e010ef          	jal	800041c2 <strcpy>
  dentry->dentry_inode = inode;
    80003058:	f404                	sd	s1,40(s0)
  if (inode) inode->ref++;
    8000305a:	c481                	beqz	s1,80003062 <alloc_vfs_dentry+0x28>
    8000305c:	40dc                	lw	a5,4(s1)
    8000305e:	2785                	addiw	a5,a5,1
    80003060:	c0dc                	sw	a5,4(s1)
  dentry->parent = parent;
    80003062:	03243823          	sd	s2,48(s0)
  dentry->d_ref = 0;
    80003066:	02042023          	sw	zero,32(s0)
}
    8000306a:	8522                	mv	a0,s0
    8000306c:	70a2                	ld	ra,40(sp)
    8000306e:	7402                	ld	s0,32(sp)
    80003070:	64e2                	ld	s1,24(sp)
    80003072:	6942                	ld	s2,16(sp)
    80003074:	69a2                	ld	s3,8(sp)
    80003076:	6145                	addi	sp,sp,48
    80003078:	8082                	ret

000000008000307a <free_vfs_dentry>:
int free_vfs_dentry(struct dentry *dentry) {
    8000307a:	1141                	addi	sp,sp,-16
    8000307c:	e406                	sd	ra,8(sp)
  if (dentry->d_ref > 0) {
    8000307e:	511c                	lw	a5,32(a0)
    80003080:	00f04863          	bgtz	a5,80003090 <free_vfs_dentry+0x16>
  free_page((void *)dentry);
    80003084:	ea8fd0ef          	jal	8000072c <free_page>
  return 0;
    80003088:	4501                	li	a0,0
}
    8000308a:	60a2                	ld	ra,8(sp)
    8000308c:	0141                	addi	sp,sp,16
    8000308e:	8082                	ret
    sprint("free_vfs_dentry: dentry is still in use!\n");
    80003090:	00005517          	auipc	a0,0x5
    80003094:	45850513          	addi	a0,a0,1112 # 800084e8 <rfs_i_ops+0x1470>
    80003098:	35b010ef          	jal	80004bf2 <sprint>
    return -1;
    8000309c:	557d                	li	a0,-1
    8000309e:	b7f5                	j	8000308a <free_vfs_dentry+0x10>

00000000800030a0 <hash_get_dentry>:
struct dentry *hash_get_dentry(struct dentry *parent, char *name) {
    800030a0:	1101                	addi	sp,sp,-32
    800030a2:	ec06                	sd	ra,24(sp)
  struct dentry_key key = {.parent = parent, .name = name};
    800030a4:	e02a                	sd	a0,0(sp)
    800030a6:	e42e                	sd	a1,8(sp)
  return (struct dentry *)dentry_hash_table.virtual_hash_get(&dentry_hash_table,
    800030a8:	858a                	mv	a1,sp
    800030aa:	0000b517          	auipc	a0,0xb
    800030ae:	ece50513          	addi	a0,a0,-306 # 8000df78 <dentry_hash_table>
    800030b2:	0000c797          	auipc	a5,0xc
    800030b6:	ade7b783          	ld	a5,-1314(a5) # 8000eb90 <dentry_hash_table+0xc18>
    800030ba:	9782                	jalr	a5
}
    800030bc:	60e2                	ld	ra,24(sp)
    800030be:	6105                	addi	sp,sp,32
    800030c0:	8082                	ret

00000000800030c2 <hash_put_dentry>:
int hash_put_dentry(struct dentry *dentry) {
    800030c2:	1101                	addi	sp,sp,-32
    800030c4:	ec06                	sd	ra,24(sp)
    800030c6:	e822                	sd	s0,16(sp)
    800030c8:	e426                	sd	s1,8(sp)
    800030ca:	84aa                	mv	s1,a0
  struct dentry_key *key = alloc_page();
    800030cc:	f10fd0ef          	jal	800007dc <alloc_page>
    800030d0:	842a                	mv	s0,a0
  key->name = dentry->name;
    800030d2:	e504                	sd	s1,8(a0)
  key->parent = dentry->parent;
    800030d4:	789c                	ld	a5,48(s1)
    800030d6:	e11c                	sd	a5,0(a0)
  int ret = dentry_hash_table.virtual_hash_put(&dentry_hash_table, key, dentry);
    800030d8:	8626                	mv	a2,s1
    800030da:	85aa                	mv	a1,a0
    800030dc:	0000b517          	auipc	a0,0xb
    800030e0:	e9c50513          	addi	a0,a0,-356 # 8000df78 <dentry_hash_table>
    800030e4:	0000c797          	auipc	a5,0xc
    800030e8:	aa47b783          	ld	a5,-1372(a5) # 8000eb88 <dentry_hash_table+0xc10>
    800030ec:	9782                	jalr	a5
    800030ee:	84aa                	mv	s1,a0
  if (ret != 0)
    800030f0:	e519                	bnez	a0,800030fe <hash_put_dentry+0x3c>
}
    800030f2:	8526                	mv	a0,s1
    800030f4:	60e2                	ld	ra,24(sp)
    800030f6:	6442                	ld	s0,16(sp)
    800030f8:	64a2                	ld	s1,8(sp)
    800030fa:	6105                	addi	sp,sp,32
    800030fc:	8082                	ret
    free_page(key);
    800030fe:	8522                	mv	a0,s0
    80003100:	e2cfd0ef          	jal	8000072c <free_page>
    80003104:	b7fd                	j	800030f2 <hash_put_dentry+0x30>

0000000080003106 <hash_erase_dentry>:
int hash_erase_dentry(struct dentry *dentry) {
    80003106:	1101                	addi	sp,sp,-32
    80003108:	ec06                	sd	ra,24(sp)
  struct dentry_key key = {.parent = dentry->parent, .name = dentry->name};
    8000310a:	791c                	ld	a5,48(a0)
    8000310c:	e03e                	sd	a5,0(sp)
    8000310e:	e42a                	sd	a0,8(sp)
  return dentry_hash_table.virtual_hash_erase(&dentry_hash_table, &key);
    80003110:	858a                	mv	a1,sp
    80003112:	0000b517          	auipc	a0,0xb
    80003116:	e6650513          	addi	a0,a0,-410 # 8000df78 <dentry_hash_table>
    8000311a:	0000c797          	auipc	a5,0xc
    8000311e:	a7e7b783          	ld	a5,-1410(a5) # 8000eb98 <dentry_hash_table+0xc20>
    80003122:	9782                	jalr	a5
}
    80003124:	60e2                	ld	ra,24(sp)
    80003126:	6105                	addi	sp,sp,32
    80003128:	8082                	ret

000000008000312a <hash_get_vinode>:

// vinode hash table interface
struct vinode *hash_get_vinode(struct super_block *sb, int inum) {
  if (inum < 0) return NULL;
    8000312a:	0205c363          	bltz	a1,80003150 <hash_get_vinode+0x26>
struct vinode *hash_get_vinode(struct super_block *sb, int inum) {
    8000312e:	1101                	addi	sp,sp,-32
    80003130:	ec06                	sd	ra,24(sp)
  struct vinode_key key = {.sb = sb, .inum = inum};
    80003132:	c02e                	sw	a1,0(sp)
    80003134:	e42a                	sd	a0,8(sp)
  return (struct vinode *)vinode_hash_table.virtual_hash_get(&vinode_hash_table,
    80003136:	858a                	mv	a1,sp
    80003138:	0000a517          	auipc	a0,0xa
    8000313c:	21850513          	addi	a0,a0,536 # 8000d350 <vinode_hash_table>
    80003140:	0000b797          	auipc	a5,0xb
    80003144:	e287b783          	ld	a5,-472(a5) # 8000df68 <vinode_hash_table+0xc18>
    80003148:	9782                	jalr	a5
                                                             &key);
}
    8000314a:	60e2                	ld	ra,24(sp)
    8000314c:	6105                	addi	sp,sp,32
    8000314e:	8082                	ret
  if (inum < 0) return NULL;
    80003150:	4501                	li	a0,0
}
    80003152:	8082                	ret

0000000080003154 <hash_put_vinode>:

int hash_put_vinode(struct vinode *vinode) {
    80003154:	1101                	addi	sp,sp,-32
    80003156:	ec06                	sd	ra,24(sp)
    80003158:	e822                	sd	s0,16(sp)
  if (vinode->inum < 0) return -1;
    8000315a:	411c                	lw	a5,0(a0)
    8000315c:	0407c363          	bltz	a5,800031a2 <hash_put_vinode+0x4e>
    80003160:	e426                	sd	s1,8(sp)
    80003162:	842a                	mv	s0,a0
  struct vinode_key *key = alloc_page();
    80003164:	e78fd0ef          	jal	800007dc <alloc_page>
    80003168:	84aa                	mv	s1,a0
  key->sb = vinode->sb;
    8000316a:	643c                	ld	a5,72(s0)
    8000316c:	e51c                	sd	a5,8(a0)
  key->inum = vinode->inum;
    8000316e:	401c                	lw	a5,0(s0)
    80003170:	c11c                	sw	a5,0(a0)

  int ret = vinode_hash_table.virtual_hash_put(&vinode_hash_table, key, vinode);
    80003172:	8622                	mv	a2,s0
    80003174:	85aa                	mv	a1,a0
    80003176:	0000a517          	auipc	a0,0xa
    8000317a:	1da50513          	addi	a0,a0,474 # 8000d350 <vinode_hash_table>
    8000317e:	0000b797          	auipc	a5,0xb
    80003182:	de27b783          	ld	a5,-542(a5) # 8000df60 <vinode_hash_table+0xc10>
    80003186:	9782                	jalr	a5
    80003188:	842a                	mv	s0,a0
  if (ret != 0) free_page(key);
    8000318a:	e519                	bnez	a0,80003198 <hash_put_vinode+0x44>
    8000318c:	64a2                	ld	s1,8(sp)
  return ret;
}
    8000318e:	8522                	mv	a0,s0
    80003190:	60e2                	ld	ra,24(sp)
    80003192:	6442                	ld	s0,16(sp)
    80003194:	6105                	addi	sp,sp,32
    80003196:	8082                	ret
  if (ret != 0) free_page(key);
    80003198:	8526                	mv	a0,s1
    8000319a:	d92fd0ef          	jal	8000072c <free_page>
    8000319e:	64a2                	ld	s1,8(sp)
    800031a0:	b7fd                	j	8000318e <hash_put_vinode+0x3a>
  if (vinode->inum < 0) return -1;
    800031a2:	547d                	li	s0,-1
    800031a4:	b7ed                	j	8000318e <hash_put_vinode+0x3a>

00000000800031a6 <vfs_mount>:
struct super_block *vfs_mount(const char *dev_name, int mnt_type) {
    800031a6:	7179                	addi	sp,sp,-48
    800031a8:	f406                	sd	ra,40(sp)
    800031aa:	f022                	sd	s0,32(sp)
    800031ac:	ec26                	sd	s1,24(sp)
    800031ae:	e84a                	sd	s2,16(sp)
    800031b0:	e44e                	sd	s3,8(sp)
    800031b2:	89aa                	mv	s3,a0
    800031b4:	892e                	mv	s2,a1
  for (int i = 0; i < MAX_VFS_DEV; ++i) {
    800031b6:	4401                	li	s0,0
  struct device *p_device = NULL;
    800031b8:	4481                	li	s1,0
  for (int i = 0; i < MAX_VFS_DEV; ++i) {
    800031ba:	a011                	j	800031be <vfs_mount+0x18>
    800031bc:	2405                	addiw	s0,s0,1
    800031be:	47a5                	li	a5,9
    800031c0:	0287c063          	blt	a5,s0,800031e0 <vfs_mount+0x3a>
    p_device = vfs_dev_list[i];
    800031c4:	00341713          	slli	a4,s0,0x3
    800031c8:	0000a797          	auipc	a5,0xa
    800031cc:	09878793          	addi	a5,a5,152 # 8000d260 <vfs_dev_list>
    800031d0:	97ba                	add	a5,a5,a4
    800031d2:	6384                	ld	s1,0(a5)
    if (p_device && strcmp(p_device->dev_name, dev_name) == 0) break;
    800031d4:	d4e5                	beqz	s1,800031bc <vfs_mount+0x16>
    800031d6:	85ce                	mv	a1,s3
    800031d8:	8526                	mv	a0,s1
    800031da:	7d1000ef          	jal	800041aa <strcmp>
    800031de:	fd79                	bnez	a0,800031bc <vfs_mount+0x16>
  if (p_device == NULL) panic("vfs_mount: cannot find the device entry!\n");
    800031e0:	cc81                	beqz	s1,800031f8 <vfs_mount+0x52>
  struct file_system_type *fs_type = p_device->fs_type;
    800031e2:	749c                	ld	a5,40(s1)
  struct super_block *sb = fs_type->get_superblock(p_device);
    800031e4:	679c                	ld	a5,8(a5)
    800031e6:	8526                	mv	a0,s1
    800031e8:	9782                	jalr	a5
    800031ea:	842a                	mv	s0,a0
  hash_put_vinode(sb->s_root->dentry_inode);
    800031ec:	691c                	ld	a5,16(a0)
    800031ee:	7788                	ld	a0,40(a5)
    800031f0:	f65ff0ef          	jal	80003154 <hash_put_vinode>
  for (int i = 0; i < MAX_MOUNTS; ++i) {
    800031f4:	4781                	li	a5,0
    800031f6:	a801                	j	80003206 <vfs_mount+0x60>
  if (p_device == NULL) panic("vfs_mount: cannot find the device entry!\n");
    800031f8:	00005517          	auipc	a0,0x5
    800031fc:	32050513          	addi	a0,a0,800 # 80008518 <rfs_i_ops+0x14a0>
    80003200:	297010ef          	jal	80004c96 <do_panic>
  for (int i = 0; i < MAX_MOUNTS; ++i) {
    80003204:	2785                	addiw	a5,a5,1
    80003206:	4725                	li	a4,9
    80003208:	06f74363          	blt	a4,a5,8000326e <vfs_mount+0xc8>
    if (vfs_sb_list[i] == NULL) {
    8000320c:	00379693          	slli	a3,a5,0x3
    80003210:	0000a717          	auipc	a4,0xa
    80003214:	05070713          	addi	a4,a4,80 # 8000d260 <vfs_dev_list>
    80003218:	9736                	add	a4,a4,a3
    8000321a:	6b38                	ld	a4,80(a4)
    8000321c:	f765                	bnez	a4,80003204 <vfs_mount+0x5e>
      vfs_sb_list[i] = sb;
    8000321e:	0000a717          	auipc	a4,0xa
    80003222:	04270713          	addi	a4,a4,66 # 8000d260 <vfs_dev_list>
    80003226:	00d707b3          	add	a5,a4,a3
    8000322a:	eba0                	sd	s0,80(a5)
  if (mnt_type == MOUNT_AS_ROOT) {
    8000322c:	4785                	li	a5,1
    8000322e:	04f90663          	beq	s2,a5,8000327a <vfs_mount+0xd4>
  } else if (mnt_type == MOUNT_DEFAULT) {
    80003232:	06091263          	bnez	s2,80003296 <vfs_mount+0xf0>
    if (!vfs_root_dentry)
    80003236:	0000d797          	auipc	a5,0xd
    8000323a:	f0a7b783          	ld	a5,-246(a5) # 80010140 <vfs_root_dentry>
    8000323e:	c7b1                	beqz	a5,8000328a <vfs_mount+0xe4>
    struct dentry *mnt_point = sb->s_root;
    80003240:	01043903          	ld	s2,16(s0)
    strcpy(mnt_point->name, dev_name);
    80003244:	85a6                	mv	a1,s1
    80003246:	854a                	mv	a0,s2
    80003248:	77b000ef          	jal	800041c2 <strcpy>
    mnt_point->parent = vfs_root_dentry;
    8000324c:	0000d797          	auipc	a5,0xd
    80003250:	ef47b783          	ld	a5,-268(a5) # 80010140 <vfs_root_dentry>
    80003254:	02f93823          	sd	a5,48(s2)
    hash_put_dentry(sb->s_root);
    80003258:	6808                	ld	a0,16(s0)
    8000325a:	e69ff0ef          	jal	800030c2 <hash_put_dentry>
}
    8000325e:	8522                	mv	a0,s0
    80003260:	70a2                	ld	ra,40(sp)
    80003262:	7402                	ld	s0,32(sp)
    80003264:	64e2                	ld	s1,24(sp)
    80003266:	6942                	ld	s2,16(sp)
    80003268:	69a2                	ld	s3,8(sp)
    8000326a:	6145                	addi	sp,sp,48
    8000326c:	8082                	ret
  if (err) panic("vfs_mount: too many mounts!\n");
    8000326e:	00005517          	auipc	a0,0x5
    80003272:	2da50513          	addi	a0,a0,730 # 80008548 <rfs_i_ops+0x14d0>
    80003276:	221010ef          	jal	80004c96 <do_panic>
    vfs_root_dentry = sb->s_root;
    8000327a:	6808                	ld	a0,16(s0)
    8000327c:	0000d797          	auipc	a5,0xd
    80003280:	eca7b223          	sd	a0,-316(a5) # 80010140 <vfs_root_dentry>
    hash_put_dentry(sb->s_root);
    80003284:	e3fff0ef          	jal	800030c2 <hash_put_dentry>
    80003288:	bfd9                	j	8000325e <vfs_mount+0xb8>
      panic("vfs_mount: root dentry not found, please mount the root device first!\n");
    8000328a:	00005517          	auipc	a0,0x5
    8000328e:	2de50513          	addi	a0,a0,734 # 80008568 <rfs_i_ops+0x14f0>
    80003292:	205010ef          	jal	80004c96 <do_panic>
    panic("vfs_mount: unknown mount type!\n");
    80003296:	00005517          	auipc	a0,0x5
    8000329a:	31a50513          	addi	a0,a0,794 # 800085b0 <rfs_i_ops+0x1538>
    8000329e:	1f9010ef          	jal	80004c96 <do_panic>

00000000800032a2 <lookup_final_dentry>:
                                   char *miss_name) {
    800032a2:	715d                	addi	sp,sp,-80
    800032a4:	e486                	sd	ra,72(sp)
    800032a6:	e0a2                	sd	s0,64(sp)
    800032a8:	fc26                	sd	s1,56(sp)
    800032aa:	f84a                	sd	s2,48(sp)
    800032ac:	f44e                	sd	s3,40(sp)
    800032ae:	f052                	sd	s4,32(sp)
    800032b0:	89ae                	mv	s3,a1
    800032b2:	8a32                	mv	s4,a2
  strcpy(path_copy, path);
    800032b4:	85aa                	mv	a1,a0
    800032b6:	850a                	mv	a0,sp
    800032b8:	70b000ef          	jal	800041c2 <strcpy>
  char *token = strtok(path_copy, "/");
    800032bc:	00004597          	auipc	a1,0x4
    800032c0:	1dc58593          	addi	a1,a1,476 # 80007498 <rfs_i_ops+0x420>
    800032c4:	850a                	mv	a0,sp
    800032c6:	725000ef          	jal	800041ea <strtok>
    800032ca:	84aa                	mv	s1,a0
  struct dentry *this = *parent;
    800032cc:	0009b403          	ld	s0,0(s3)
  while (token != NULL) {
    800032d0:	a0b9                	j	8000331e <lookup_final_dentry+0x7c>
        free_page(this);
    800032d2:	8522                	mv	a0,s0
    800032d4:	c58fd0ef          	jal	8000072c <free_page>
        strcpy(miss_name, token);
    800032d8:	85a6                	mv	a1,s1
    800032da:	8552                	mv	a0,s4
    800032dc:	6e7000ef          	jal	800041c2 <strcpy>
        return NULL;
    800032e0:	844a                	mv	s0,s2
}
    800032e2:	8522                	mv	a0,s0
    800032e4:	60a6                	ld	ra,72(sp)
    800032e6:	6406                	ld	s0,64(sp)
    800032e8:	74e2                	ld	s1,56(sp)
    800032ea:	7942                	ld	s2,48(sp)
    800032ec:	79a2                	ld	s3,40(sp)
    800032ee:	7a02                	ld	s4,32(sp)
    800032f0:	6161                	addi	sp,sp,80
    800032f2:	8082                	ret
        this->dentry_inode = found_vinode;
    800032f4:	03243423          	sd	s2,40(s0)
        found_vinode->ref++;
    800032f8:	00492783          	lw	a5,4(s2)
    800032fc:	2785                	addiw	a5,a5,1
    800032fe:	00f92223          	sw	a5,4(s2)
        hash_put_vinode(found_vinode);
    80003302:	854a                	mv	a0,s2
    80003304:	e51ff0ef          	jal	80003154 <hash_put_vinode>
      hash_put_dentry(this);
    80003308:	8522                	mv	a0,s0
    8000330a:	db9ff0ef          	jal	800030c2 <hash_put_dentry>
    token = strtok(NULL, "/");
    8000330e:	00004597          	auipc	a1,0x4
    80003312:	18a58593          	addi	a1,a1,394 # 80007498 <rfs_i_ops+0x420>
    80003316:	4501                	li	a0,0
    80003318:	6d3000ef          	jal	800041ea <strtok>
    8000331c:	84aa                	mv	s1,a0
  while (token != NULL) {
    8000331e:	d0f1                	beqz	s1,800032e2 <lookup_final_dentry+0x40>
    *parent = this;
    80003320:	0089b023          	sd	s0,0(s3)
    this = hash_get_dentry((*parent), token);  // try hash first
    80003324:	85a6                	mv	a1,s1
    80003326:	8522                	mv	a0,s0
    80003328:	d79ff0ef          	jal	800030a0 <hash_get_dentry>
    8000332c:	842a                	mv	s0,a0
    if (this == NULL) {
    8000332e:	f165                	bnez	a0,8000330e <lookup_final_dentry+0x6c>
      this = alloc_vfs_dentry(token, NULL, *parent);
    80003330:	0009b603          	ld	a2,0(s3)
    80003334:	4581                	li	a1,0
    80003336:	8526                	mv	a0,s1
    80003338:	d03ff0ef          	jal	8000303a <alloc_vfs_dentry>
    8000333c:	842a                	mv	s0,a0
      struct vinode *found_vinode = viop_lookup((*parent)->dentry_inode, this);
    8000333e:	0009b783          	ld	a5,0(s3)
    80003342:	7788                	ld	a0,40(a5)
    80003344:	693c                	ld	a5,80(a0)
    80003346:	7f9c                	ld	a5,56(a5)
    80003348:	85a2                	mv	a1,s0
    8000334a:	9782                	jalr	a5
    8000334c:	892a                	mv	s2,a0
      if (found_vinode == NULL) {
    8000334e:	d151                	beqz	a0,800032d2 <lookup_final_dentry+0x30>
      struct vinode *same_inode = hash_get_vinode(found_vinode->sb, found_vinode->inum);
    80003350:	410c                	lw	a1,0(a0)
    80003352:	6528                	ld	a0,72(a0)
    80003354:	dd7ff0ef          	jal	8000312a <hash_get_vinode>
      if (same_inode != NULL) {
    80003358:	dd51                	beqz	a0,800032f4 <lookup_final_dentry+0x52>
        this->dentry_inode = same_inode;
    8000335a:	f408                	sd	a0,40(s0)
        same_inode->ref++;
    8000335c:	415c                	lw	a5,4(a0)
    8000335e:	2785                	addiw	a5,a5,1
    80003360:	c15c                	sw	a5,4(a0)
        free_page(found_vinode);
    80003362:	854a                	mv	a0,s2
    80003364:	bc8fd0ef          	jal	8000072c <free_page>
    80003368:	b745                	j	80003308 <lookup_final_dentry+0x66>

000000008000336a <vfs_link>:
int vfs_link(const char *oldpath, const char *newpath) {
    8000336a:	7159                	addi	sp,sp,-112
    8000336c:	f486                	sd	ra,104(sp)
    8000336e:	f0a2                	sd	s0,96(sp)
    80003370:	eca6                	sd	s1,88(sp)
    80003372:	84ae                	mv	s1,a1
  struct dentry *parent = vfs_root_dentry;
    80003374:	0000d797          	auipc	a5,0xd
    80003378:	dcc7b783          	ld	a5,-564(a5) # 80010140 <vfs_root_dentry>
    8000337c:	e4be                	sd	a5,72(sp)
      lookup_final_dentry(oldpath, &parent, miss_name);
    8000337e:	1030                	addi	a2,sp,40
    80003380:	00ac                	addi	a1,sp,72
    80003382:	f21ff0ef          	jal	800032a2 <lookup_final_dentry>
  if (!old_file_dentry) {
    80003386:	c12d                	beqz	a0,800033e8 <vfs_link+0x7e>
    80003388:	842a                	mv	s0,a0
  if (old_file_dentry->dentry_inode->type != FILE_I) {
    8000338a:	751c                	ld	a5,40(a0)
    8000338c:	47dc                	lw	a5,12(a5)
    8000338e:	e7ad                	bnez	a5,800033f8 <vfs_link+0x8e>
  parent = vfs_root_dentry;
    80003390:	0000d797          	auipc	a5,0xd
    80003394:	db07b783          	ld	a5,-592(a5) # 80010140 <vfs_root_dentry>
    80003398:	e4be                	sd	a5,72(sp)
      lookup_final_dentry(newpath, &parent, miss_name);
    8000339a:	1030                	addi	a2,sp,40
    8000339c:	00ac                	addi	a1,sp,72
    8000339e:	8526                	mv	a0,s1
    800033a0:	f03ff0ef          	jal	800032a2 <lookup_final_dentry>
  if (new_file_dentry) {
    800033a4:	e135                	bnez	a0,80003408 <vfs_link+0x9e>
  get_base_name(newpath, basename);
    800033a6:	002c                	addi	a1,sp,8
    800033a8:	8526                	mv	a0,s1
    800033aa:	c11ff0ef          	jal	80002fba <get_base_name>
  if (strcmp(miss_name, basename) != 0) {
    800033ae:	002c                	addi	a1,sp,8
    800033b0:	1028                	addi	a0,sp,40
    800033b2:	5f9000ef          	jal	800041aa <strcmp>
    800033b6:	e12d                	bnez	a0,80003418 <vfs_link+0xae>
  new_file_dentry = alloc_vfs_dentry(basename, old_file_dentry->dentry_inode, parent);
    800033b8:	6626                	ld	a2,72(sp)
    800033ba:	740c                	ld	a1,40(s0)
    800033bc:	0028                	addi	a0,sp,8
    800033be:	c7dff0ef          	jal	8000303a <alloc_vfs_dentry>
    800033c2:	84aa                	mv	s1,a0
      viop_link(parent->dentry_inode, new_file_dentry, old_file_dentry->dentry_inode);
    800033c4:	67a6                	ld	a5,72(sp)
    800033c6:	7788                	ld	a0,40(a5)
    800033c8:	693c                	ld	a5,80(a0)
    800033ca:	779c                	ld	a5,40(a5)
    800033cc:	7410                	ld	a2,40(s0)
    800033ce:	85a6                	mv	a1,s1
    800033d0:	9782                	jalr	a5
    800033d2:	842a                	mv	s0,a0
  if (err) return -1;
    800033d4:	e931                	bnez	a0,80003428 <vfs_link+0xbe>
  hash_put_dentry(new_file_dentry);
    800033d6:	8526                	mv	a0,s1
    800033d8:	cebff0ef          	jal	800030c2 <hash_put_dentry>
}
    800033dc:	8522                	mv	a0,s0
    800033de:	70a6                	ld	ra,104(sp)
    800033e0:	7406                	ld	s0,96(sp)
    800033e2:	64e6                	ld	s1,88(sp)
    800033e4:	6165                	addi	sp,sp,112
    800033e6:	8082                	ret
    sprint("vfs_link: cannot find the file!\n");
    800033e8:	00005517          	auipc	a0,0x5
    800033ec:	1f050513          	addi	a0,a0,496 # 800085d8 <rfs_i_ops+0x1560>
    800033f0:	003010ef          	jal	80004bf2 <sprint>
    return -1;
    800033f4:	547d                	li	s0,-1
    800033f6:	b7dd                	j	800033dc <vfs_link+0x72>
    sprint("vfs_link: cannot link a directory!\n");
    800033f8:	00005517          	auipc	a0,0x5
    800033fc:	20850513          	addi	a0,a0,520 # 80008600 <rfs_i_ops+0x1588>
    80003400:	7f2010ef          	jal	80004bf2 <sprint>
    return -1;
    80003404:	547d                	li	s0,-1
    80003406:	bfd9                	j	800033dc <vfs_link+0x72>
    sprint("vfs_link: the new file already exists!\n");
    80003408:	00005517          	auipc	a0,0x5
    8000340c:	22050513          	addi	a0,a0,544 # 80008628 <rfs_i_ops+0x15b0>
    80003410:	7e2010ef          	jal	80004bf2 <sprint>
    return -1;
    80003414:	547d                	li	s0,-1
    80003416:	b7d9                	j	800033dc <vfs_link+0x72>
    sprint("vfs_link: cannot create file in a non-exist directory!\n");
    80003418:	00005517          	auipc	a0,0x5
    8000341c:	23850513          	addi	a0,a0,568 # 80008650 <rfs_i_ops+0x15d8>
    80003420:	7d2010ef          	jal	80004bf2 <sprint>
    return -1;
    80003424:	547d                	li	s0,-1
    80003426:	bf5d                	j	800033dc <vfs_link+0x72>
  if (err) return -1;
    80003428:	547d                	li	s0,-1
    8000342a:	bf4d                	j	800033dc <vfs_link+0x72>

000000008000342c <vfs_opendir>:
struct file *vfs_opendir(const char *path) {
    8000342c:	715d                	addi	sp,sp,-80
    8000342e:	e486                	sd	ra,72(sp)
    80003430:	fc26                	sd	s1,56(sp)
  struct dentry *parent = vfs_root_dentry;
    80003432:	0000d797          	auipc	a5,0xd
    80003436:	d0e7b783          	ld	a5,-754(a5) # 80010140 <vfs_root_dentry>
    8000343a:	f43e                	sd	a5,40(sp)
  struct dentry *file_dentry = lookup_final_dentry(path, &parent, miss_name);
    8000343c:	0030                	addi	a2,sp,8
    8000343e:	102c                	addi	a1,sp,40
    80003440:	e63ff0ef          	jal	800032a2 <lookup_final_dentry>
  if (!file_dentry || file_dentry->dentry_inode->type != DIR_I) {
    80003444:	c905                	beqz	a0,80003474 <vfs_opendir+0x48>
    80003446:	e0a2                	sd	s0,64(sp)
    80003448:	842a                	mv	s0,a0
    8000344a:	751c                	ld	a5,40(a0)
    8000344c:	47d8                	lw	a4,12(a5)
    8000344e:	4785                	li	a5,1
    80003450:	02f71163          	bne	a4,a5,80003472 <vfs_opendir+0x46>
  struct file *file = alloc_vfs_file(file_dentry, 1, 0, 0);
    80003454:	4681                	li	a3,0
    80003456:	4601                	li	a2,0
    80003458:	4585                	li	a1,1
    8000345a:	babff0ef          	jal	80003004 <alloc_vfs_file>
    8000345e:	84aa                	mv	s1,a0
  if (file_dentry->dentry_inode->i_ops->viop_hook_opendir) {
    80003460:	7408                	ld	a0,40(s0)
    80003462:	693c                	ld	a5,80(a0)
    80003464:	77bc                	ld	a5,104(a5)
    80003466:	cb9d                	beqz	a5,8000349c <vfs_opendir+0x70>
    if (file_dentry->dentry_inode->i_ops->
    80003468:	85a2                	mv	a1,s0
    8000346a:	9782                	jalr	a5
    8000346c:	e105                	bnez	a0,8000348c <vfs_opendir+0x60>
    8000346e:	6406                	ld	s0,64(sp)
    80003470:	a809                	j	80003482 <vfs_opendir+0x56>
    80003472:	6406                	ld	s0,64(sp)
    sprint("vfs_opendir: cannot find the direntry!\n");
    80003474:	00005517          	auipc	a0,0x5
    80003478:	21450513          	addi	a0,a0,532 # 80008688 <rfs_i_ops+0x1610>
    8000347c:	776010ef          	jal	80004bf2 <sprint>
    return NULL;
    80003480:	4481                	li	s1,0
}
    80003482:	8526                	mv	a0,s1
    80003484:	60a6                	ld	ra,72(sp)
    80003486:	74e2                	ld	s1,56(sp)
    80003488:	6161                	addi	sp,sp,80
    8000348a:	8082                	ret
      sprint("vfs_opendir: hook opendir failed!\n");
    8000348c:	00005517          	auipc	a0,0x5
    80003490:	22450513          	addi	a0,a0,548 # 800086b0 <rfs_i_ops+0x1638>
    80003494:	75e010ef          	jal	80004bf2 <sprint>
    80003498:	6406                	ld	s0,64(sp)
    8000349a:	b7e5                	j	80003482 <vfs_opendir+0x56>
    8000349c:	6406                	ld	s0,64(sp)
    8000349e:	b7d5                	j	80003482 <vfs_opendir+0x56>

00000000800034a0 <vfs_open>:
struct file *vfs_open(const char *path, int flags) {
    800034a0:	7159                	addi	sp,sp,-112
    800034a2:	f486                	sd	ra,104(sp)
    800034a4:	f0a2                	sd	s0,96(sp)
    800034a6:	eca6                	sd	s1,88(sp)
    800034a8:	e8ca                	sd	s2,80(sp)
    800034aa:	892a                	mv	s2,a0
    800034ac:	84ae                	mv	s1,a1
  struct dentry *parent = vfs_root_dentry; // we start the path lookup from root.
    800034ae:	0000d797          	auipc	a5,0xd
    800034b2:	c927b783          	ld	a5,-878(a5) # 80010140 <vfs_root_dentry>
    800034b6:	e4be                	sd	a5,72(sp)
  struct dentry *file_dentry = lookup_final_dentry(path, &parent, miss_name);
    800034b8:	1030                	addi	a2,sp,40
    800034ba:	00ac                	addi	a1,sp,72
    800034bc:	de7ff0ef          	jal	800032a2 <lookup_final_dentry>
    800034c0:	842a                	mv	s0,a0
  if (!file_dentry) {
    800034c2:	cd19                	beqz	a0,800034e0 <vfs_open+0x40>
  if (file_dentry->dentry_inode->type != FILE_I) {
    800034c4:	741c                	ld	a5,40(s0)
    800034c6:	47cc                	lw	a1,12(a5)
    800034c8:	e5d9                	bnez	a1,80003556 <vfs_open+0xb6>
  switch (flags & MASK_FILEMODE) {
    800034ca:	0034f613          	andi	a2,s1,3
    800034ce:	4785                	li	a5,1
    800034d0:	0af60363          	beq	a2,a5,80003576 <vfs_open+0xd6>
    800034d4:	4789                	li	a5,2
    800034d6:	08f60e63          	beq	a2,a5,80003572 <vfs_open+0xd2>
    800034da:	e651                	bnez	a2,80003566 <vfs_open+0xc6>
    800034dc:	4585                	li	a1,1
    800034de:	a861                	j	80003576 <vfs_open+0xd6>
    int creatable = flags & O_CREAT;
    800034e0:	0404f793          	andi	a5,s1,64
    if (creatable) {
    800034e4:	c3ad                	beqz	a5,80003546 <vfs_open+0xa6>
      get_base_name(path, basename);
    800034e6:	002c                	addi	a1,sp,8
    800034e8:	854a                	mv	a0,s2
    800034ea:	ad1ff0ef          	jal	80002fba <get_base_name>
      if (strcmp(miss_name, basename) != 0) {
    800034ee:	002c                	addi	a1,sp,8
    800034f0:	1028                	addi	a0,sp,40
    800034f2:	4b9000ef          	jal	800041aa <strcmp>
    800034f6:	e915                	bnez	a0,8000352a <vfs_open+0x8a>
      file_dentry = alloc_vfs_dentry(basename, NULL, parent);
    800034f8:	6626                	ld	a2,72(sp)
    800034fa:	4581                	li	a1,0
    800034fc:	0028                	addi	a0,sp,8
    800034fe:	b3dff0ef          	jal	8000303a <alloc_vfs_dentry>
    80003502:	842a                	mv	s0,a0
      struct vinode *new_inode = viop_create(parent->dentry_inode, file_dentry);
    80003504:	67a6                	ld	a5,72(sp)
    80003506:	7788                	ld	a0,40(a5)
    80003508:	693c                	ld	a5,80(a0)
    8000350a:	6b9c                	ld	a5,16(a5)
    8000350c:	85a2                	mv	a1,s0
    8000350e:	9782                	jalr	a5
    80003510:	892a                	mv	s2,a0
      if (!new_inode) panic("vfs_open: cannot create file!\n");
    80003512:	c505                	beqz	a0,8000353a <vfs_open+0x9a>
      file_dentry->dentry_inode = new_inode;
    80003514:	f408                	sd	a0,40(s0)
      new_inode->ref++;
    80003516:	415c                	lw	a5,4(a0)
    80003518:	2785                	addiw	a5,a5,1
    8000351a:	c15c                	sw	a5,4(a0)
      hash_put_dentry(file_dentry);
    8000351c:	8522                	mv	a0,s0
    8000351e:	ba5ff0ef          	jal	800030c2 <hash_put_dentry>
      hash_put_vinode(new_inode); 
    80003522:	854a                	mv	a0,s2
    80003524:	c31ff0ef          	jal	80003154 <hash_put_vinode>
    80003528:	bf71                	j	800034c4 <vfs_open+0x24>
        sprint("vfs_open: cannot create file in a non-exist directory!\n");
    8000352a:	00005517          	auipc	a0,0x5
    8000352e:	1ae50513          	addi	a0,a0,430 # 800086d8 <rfs_i_ops+0x1660>
    80003532:	6c0010ef          	jal	80004bf2 <sprint>
        return NULL;
    80003536:	84a2                	mv	s1,s0
    80003538:	a8a1                	j	80003590 <vfs_open+0xf0>
      if (!new_inode) panic("vfs_open: cannot create file!\n");
    8000353a:	00005517          	auipc	a0,0x5
    8000353e:	1d650513          	addi	a0,a0,470 # 80008710 <rfs_i_ops+0x1698>
    80003542:	754010ef          	jal	80004c96 <do_panic>
      sprint("vfs_open: cannot find the file!\n");
    80003546:	00005517          	auipc	a0,0x5
    8000354a:	1ea50513          	addi	a0,a0,490 # 80008730 <rfs_i_ops+0x16b8>
    8000354e:	6a4010ef          	jal	80004bf2 <sprint>
      return NULL;
    80003552:	84a2                	mv	s1,s0
    80003554:	a835                	j	80003590 <vfs_open+0xf0>
    sprint("vfs_open: cannot open a directory!\n");
    80003556:	00005517          	auipc	a0,0x5
    8000355a:	20250513          	addi	a0,a0,514 # 80008758 <rfs_i_ops+0x16e0>
    8000355e:	694010ef          	jal	80004bf2 <sprint>
    return NULL;
    80003562:	4481                	li	s1,0
    80003564:	a035                	j	80003590 <vfs_open+0xf0>
      panic("fs_open: invalid open flags!\n");
    80003566:	00005517          	auipc	a0,0x5
    8000356a:	21a50513          	addi	a0,a0,538 # 80008780 <rfs_i_ops+0x1708>
    8000356e:	728010ef          	jal	80004c96 <do_panic>
      readable = 1;
    80003572:	4585                	li	a1,1
      writable = 1;
    80003574:	4605                	li	a2,1
  struct file *file = alloc_vfs_file(file_dentry, readable, writable, 0);
    80003576:	4681                	li	a3,0
    80003578:	8522                	mv	a0,s0
    8000357a:	a8bff0ef          	jal	80003004 <alloc_vfs_file>
    8000357e:	84aa                	mv	s1,a0
  if (file_dentry->dentry_inode->i_ops->viop_hook_open) {
    80003580:	7408                	ld	a0,40(s0)
    80003582:	693c                	ld	a5,80(a0)
    80003584:	6fbc                	ld	a5,88(a5)
    80003586:	c789                	beqz	a5,80003590 <vfs_open+0xf0>
    if (file_dentry->dentry_inode->i_ops->
    80003588:	85a2                	mv	a1,s0
    8000358a:	9782                	jalr	a5
    8000358c:	00054963          	bltz	a0,8000359e <vfs_open+0xfe>
}
    80003590:	8526                	mv	a0,s1
    80003592:	70a6                	ld	ra,104(sp)
    80003594:	7406                	ld	s0,96(sp)
    80003596:	64e6                	ld	s1,88(sp)
    80003598:	6946                	ld	s2,80(sp)
    8000359a:	6165                	addi	sp,sp,112
    8000359c:	8082                	ret
      sprint("vfs_open: hook_open failed!\n");
    8000359e:	00005517          	auipc	a0,0x5
    800035a2:	20250513          	addi	a0,a0,514 # 800087a0 <rfs_i_ops+0x1728>
    800035a6:	64c010ef          	jal	80004bf2 <sprint>
    800035aa:	b7dd                	j	80003590 <vfs_open+0xf0>

00000000800035ac <vfs_mkdir>:
int vfs_mkdir(const char *path) {
    800035ac:	7159                	addi	sp,sp,-112
    800035ae:	f486                	sd	ra,104(sp)
    800035b0:	f0a2                	sd	s0,96(sp)
    800035b2:	e8ca                	sd	s2,80(sp)
    800035b4:	842a                	mv	s0,a0
  struct dentry *parent = vfs_root_dentry;
    800035b6:	0000d797          	auipc	a5,0xd
    800035ba:	b8a7b783          	ld	a5,-1142(a5) # 80010140 <vfs_root_dentry>
    800035be:	e4be                	sd	a5,72(sp)
  struct dentry *file_dentry = lookup_final_dentry(path, &parent, miss_name);
    800035c0:	1030                	addi	a2,sp,40
    800035c2:	00ac                	addi	a1,sp,72
    800035c4:	cdfff0ef          	jal	800032a2 <lookup_final_dentry>
  if (file_dentry) {
    800035c8:	e939                	bnez	a0,8000361e <vfs_mkdir+0x72>
  get_base_name(path, basename);
    800035ca:	002c                	addi	a1,sp,8
    800035cc:	8522                	mv	a0,s0
    800035ce:	9edff0ef          	jal	80002fba <get_base_name>
  if (strcmp(miss_name, basename) != 0) {
    800035d2:	002c                	addi	a1,sp,8
    800035d4:	1028                	addi	a0,sp,40
    800035d6:	3d5000ef          	jal	800041aa <strcmp>
    800035da:	892a                	mv	s2,a0
    800035dc:	e929                	bnez	a0,8000362e <vfs_mkdir+0x82>
    800035de:	eca6                	sd	s1,88(sp)
  struct dentry *new_dentry = alloc_vfs_dentry(basename, NULL, parent);
    800035e0:	6626                	ld	a2,72(sp)
    800035e2:	4581                	li	a1,0
    800035e4:	0028                	addi	a0,sp,8
    800035e6:	a55ff0ef          	jal	8000303a <alloc_vfs_dentry>
    800035ea:	84aa                	mv	s1,a0
  struct vinode *new_dir_inode = viop_mkdir(parent->dentry_inode, new_dentry);
    800035ec:	67a6                	ld	a5,72(sp)
    800035ee:	7788                	ld	a0,40(a5)
    800035f0:	693c                	ld	a5,80(a0)
    800035f2:	67bc                	ld	a5,72(a5)
    800035f4:	85a6                	mv	a1,s1
    800035f6:	9782                	jalr	a5
    800035f8:	842a                	mv	s0,a0
  if (!new_dir_inode) {
    800035fa:	c131                	beqz	a0,8000363e <vfs_mkdir+0x92>
  new_dentry->dentry_inode = new_dir_inode;
    800035fc:	f488                	sd	a0,40(s1)
  new_dir_inode->ref++;
    800035fe:	415c                	lw	a5,4(a0)
    80003600:	2785                	addiw	a5,a5,1
    80003602:	c15c                	sw	a5,4(a0)
  hash_put_dentry(new_dentry);
    80003604:	8526                	mv	a0,s1
    80003606:	abdff0ef          	jal	800030c2 <hash_put_dentry>
  hash_put_vinode(new_dir_inode);
    8000360a:	8522                	mv	a0,s0
    8000360c:	b49ff0ef          	jal	80003154 <hash_put_vinode>
    80003610:	64e6                	ld	s1,88(sp)
}
    80003612:	854a                	mv	a0,s2
    80003614:	70a6                	ld	ra,104(sp)
    80003616:	7406                	ld	s0,96(sp)
    80003618:	6946                	ld	s2,80(sp)
    8000361a:	6165                	addi	sp,sp,112
    8000361c:	8082                	ret
    sprint("vfs_mkdir: the directory already exists!\n");
    8000361e:	00005517          	auipc	a0,0x5
    80003622:	1a250513          	addi	a0,a0,418 # 800087c0 <rfs_i_ops+0x1748>
    80003626:	5cc010ef          	jal	80004bf2 <sprint>
    return -1;
    8000362a:	597d                	li	s2,-1
    8000362c:	b7dd                	j	80003612 <vfs_mkdir+0x66>
    sprint("vfs_mkdir: cannot create directory in a non-exist directory!\n");
    8000362e:	00005517          	auipc	a0,0x5
    80003632:	1c250513          	addi	a0,a0,450 # 800087f0 <rfs_i_ops+0x1778>
    80003636:	5bc010ef          	jal	80004bf2 <sprint>
    return -1;
    8000363a:	597d                	li	s2,-1
    8000363c:	bfd9                	j	80003612 <vfs_mkdir+0x66>
    free_page(new_dentry);
    8000363e:	8526                	mv	a0,s1
    80003640:	8ecfd0ef          	jal	8000072c <free_page>
    sprint("vfs_mkdir: cannot create directory!\n");
    80003644:	00005517          	auipc	a0,0x5
    80003648:	1ec50513          	addi	a0,a0,492 # 80008830 <rfs_i_ops+0x17b8>
    8000364c:	5a6010ef          	jal	80004bf2 <sprint>
    return -1;
    80003650:	597d                	li	s2,-1
    80003652:	64e6                	ld	s1,88(sp)
    80003654:	bf7d                	j	80003612 <vfs_mkdir+0x66>

0000000080003656 <hash_erase_vinode>:

int hash_erase_vinode(struct vinode *vinode) {
  if (vinode->inum < 0) return -1;
    80003656:	411c                	lw	a5,0(a0)
    80003658:	0207c463          	bltz	a5,80003680 <hash_erase_vinode+0x2a>
int hash_erase_vinode(struct vinode *vinode) {
    8000365c:	1101                	addi	sp,sp,-32
    8000365e:	ec06                	sd	ra,24(sp)
  struct vinode_key key = {.sb = vinode->sb, .inum = vinode->inum};
    80003660:	c03e                	sw	a5,0(sp)
    80003662:	653c                	ld	a5,72(a0)
    80003664:	e43e                	sd	a5,8(sp)
  return vinode_hash_table.virtual_hash_erase(&vinode_hash_table, &key);
    80003666:	858a                	mv	a1,sp
    80003668:	0000a517          	auipc	a0,0xa
    8000366c:	ce850513          	addi	a0,a0,-792 # 8000d350 <vinode_hash_table>
    80003670:	0000b797          	auipc	a5,0xb
    80003674:	9007b783          	ld	a5,-1792(a5) # 8000df70 <vinode_hash_table+0xc20>
    80003678:	9782                	jalr	a5
}
    8000367a:	60e2                	ld	ra,24(sp)
    8000367c:	6105                	addi	sp,sp,32
    8000367e:	8082                	ret
  if (vinode->inum < 0) return -1;
    80003680:	557d                	li	a0,-1
}
    80003682:	8082                	ret

0000000080003684 <vfs_unlink>:
int vfs_unlink(const char *path) {
    80003684:	715d                	addi	sp,sp,-80
    80003686:	e486                	sd	ra,72(sp)
    80003688:	f84a                	sd	s2,48(sp)
  struct dentry *parent = vfs_root_dentry;
    8000368a:	0000d797          	auipc	a5,0xd
    8000368e:	ab67b783          	ld	a5,-1354(a5) # 80010140 <vfs_root_dentry>
    80003692:	f43e                	sd	a5,40(sp)
  struct dentry *file_dentry = lookup_final_dentry(path, &parent, miss_name);
    80003694:	0030                	addi	a2,sp,8
    80003696:	102c                	addi	a1,sp,40
    80003698:	c0bff0ef          	jal	800032a2 <lookup_final_dentry>
  if (!file_dentry) {
    8000369c:	c939                	beqz	a0,800036f2 <vfs_unlink+0x6e>
    8000369e:	e0a2                	sd	s0,64(sp)
    800036a0:	fc26                	sd	s1,56(sp)
    800036a2:	842a                	mv	s0,a0
  if (file_dentry->dentry_inode->type != FILE_I) {
    800036a4:	7504                	ld	s1,40(a0)
    800036a6:	44dc                	lw	a5,12(s1)
    800036a8:	efa9                	bnez	a5,80003702 <vfs_unlink+0x7e>
  if (file_dentry->d_ref > 0) {
    800036aa:	511c                	lw	a5,32(a0)
    800036ac:	06f04563          	bgtz	a5,80003716 <vfs_unlink+0x92>
  int err = viop_unlink(parent->dentry_inode, file_dentry, unlinked_vinode);
    800036b0:	77a2                	ld	a5,40(sp)
    800036b2:	7788                	ld	a0,40(a5)
    800036b4:	693c                	ld	a5,80(a0)
    800036b6:	7b9c                	ld	a5,48(a5)
    800036b8:	8626                	mv	a2,s1
    800036ba:	85a2                	mv	a1,s0
    800036bc:	9782                	jalr	a5
    800036be:	892a                	mv	s2,a0
  if (err) return -1;
    800036c0:	e949                	bnez	a0,80003752 <vfs_unlink+0xce>
  hash_erase_dentry(file_dentry);
    800036c2:	8522                	mv	a0,s0
    800036c4:	a43ff0ef          	jal	80003106 <hash_erase_dentry>
  free_vfs_dentry(file_dentry);
    800036c8:	8522                	mv	a0,s0
    800036ca:	9b1ff0ef          	jal	8000307a <free_vfs_dentry>
  unlinked_vinode->ref--; 
    800036ce:	40dc                	lw	a5,4(s1)
    800036d0:	37fd                	addiw	a5,a5,-1
    800036d2:	0007841b          	sext.w	s0,a5
    800036d6:	c0dc                	sw	a5,4(s1)
  if (unlinked_vinode->nlinks == 0) {
    800036d8:	489c                	lw	a5,16(s1)
    800036da:	e3c1                	bnez	a5,8000375a <vfs_unlink+0xd6>
    assert(unlinked_vinode->ref == 0);
    800036dc:	e439                	bnez	s0,8000372a <vfs_unlink+0xa6>
    hash_erase_vinode(unlinked_vinode);
    800036de:	8526                	mv	a0,s1
    800036e0:	f77ff0ef          	jal	80003656 <hash_erase_vinode>
    free_page(unlinked_vinode);  // free the vinode
    800036e4:	8526                	mv	a0,s1
    800036e6:	846fd0ef          	jal	8000072c <free_page>
  return 0;
    800036ea:	8922                	mv	s2,s0
    800036ec:	6406                	ld	s0,64(sp)
    800036ee:	74e2                	ld	s1,56(sp)
    800036f0:	a0bd                	j	8000375e <vfs_unlink+0xda>
    sprint("vfs_unlink: cannot find the file!\n");
    800036f2:	00005517          	auipc	a0,0x5
    800036f6:	16650513          	addi	a0,a0,358 # 80008858 <rfs_i_ops+0x17e0>
    800036fa:	4f8010ef          	jal	80004bf2 <sprint>
    return -1;
    800036fe:	597d                	li	s2,-1
    80003700:	a8b9                	j	8000375e <vfs_unlink+0xda>
    sprint("vfs_unlink: cannot unlink a directory!\n");
    80003702:	00005517          	auipc	a0,0x5
    80003706:	17e50513          	addi	a0,a0,382 # 80008880 <rfs_i_ops+0x1808>
    8000370a:	4e8010ef          	jal	80004bf2 <sprint>
    return -1;
    8000370e:	597d                	li	s2,-1
    80003710:	6406                	ld	s0,64(sp)
    80003712:	74e2                	ld	s1,56(sp)
    80003714:	a0a9                	j	8000375e <vfs_unlink+0xda>
    sprint("vfs_unlink: the file is still opened!\n");
    80003716:	00005517          	auipc	a0,0x5
    8000371a:	19250513          	addi	a0,a0,402 # 800088a8 <rfs_i_ops+0x1830>
    8000371e:	4d4010ef          	jal	80004bf2 <sprint>
    return -1;
    80003722:	597d                	li	s2,-1
    80003724:	6406                	ld	s0,64(sp)
    80003726:	74e2                	ld	s1,56(sp)
    80003728:	a81d                	j	8000375e <vfs_unlink+0xda>
    assert(unlinked_vinode->ref == 0);
    8000372a:	00005697          	auipc	a3,0x5
    8000372e:	1a668693          	addi	a3,a3,422 # 800088d0 <rfs_i_ops+0x1858>
    80003732:	15a00613          	li	a2,346
    80003736:	00005597          	auipc	a1,0x5
    8000373a:	1ba58593          	addi	a1,a1,442 # 800088f0 <rfs_i_ops+0x1878>
    8000373e:	00004517          	auipc	a0,0x4
    80003742:	04a50513          	addi	a0,a0,74 # 80007788 <rfs_i_ops+0x710>
    80003746:	4ac010ef          	jal	80004bf2 <sprint>
    8000374a:	6541                	lui	a0,0x10
    8000374c:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    8000374e:	4c4010ef          	jal	80004c12 <poweroff>
  if (err) return -1;
    80003752:	597d                	li	s2,-1
    80003754:	6406                	ld	s0,64(sp)
    80003756:	74e2                	ld	s1,56(sp)
    80003758:	a019                	j	8000375e <vfs_unlink+0xda>
    8000375a:	6406                	ld	s0,64(sp)
    8000375c:	74e2                	ld	s1,56(sp)
}
    8000375e:	854a                	mv	a0,s2
    80003760:	60a6                	ld	ra,72(sp)
    80003762:	7942                	ld	s2,48(sp)
    80003764:	6161                	addi	sp,sp,80
    80003766:	8082                	ret

0000000080003768 <vfs_close>:
int vfs_close(struct file *file) {
    80003768:	7179                	addi	sp,sp,-48
    8000376a:	f406                	sd	ra,40(sp)
    8000376c:	f022                	sd	s0,32(sp)
    8000376e:	ec26                	sd	s1,24(sp)
    80003770:	e44e                	sd	s3,8(sp)
  if (file->f_dentry->dentry_inode->type != FILE_I) {
    80003772:	6900                	ld	s0,16(a0)
    80003774:	7404                	ld	s1,40(s0)
    80003776:	00c4a983          	lw	s3,12(s1)
    8000377a:	02099b63          	bnez	s3,800037b0 <vfs_close+0x48>
    8000377e:	e84a                	sd	s2,16(sp)
    80003780:	892a                	mv	s2,a0
  if (inode->i_ops->viop_hook_close) {
    80003782:	68bc                	ld	a5,80(s1)
    80003784:	73bc                	ld	a5,96(a5)
    80003786:	c789                	beqz	a5,80003790 <vfs_close+0x28>
    if (inode->i_ops->viop_hook_close(inode, dentry) != 0) {
    80003788:	85a2                	mv	a1,s0
    8000378a:	8526                	mv	a0,s1
    8000378c:	9782                	jalr	a5
    8000378e:	e90d                	bnez	a0,800037c0 <vfs_close+0x58>
  dentry->d_ref--;
    80003790:	501c                	lw	a5,32(s0)
    80003792:	37fd                	addiw	a5,a5,-1
    80003794:	0007871b          	sext.w	a4,a5
    80003798:	d01c                	sw	a5,32(s0)
  if (dentry->d_ref == 0) {
    8000379a:	cb15                	beqz	a4,800037ce <vfs_close+0x66>
  file->status = FD_NONE;
    8000379c:	00092023          	sw	zero,0(s2)
    800037a0:	6942                	ld	s2,16(sp)
}
    800037a2:	854e                	mv	a0,s3
    800037a4:	70a2                	ld	ra,40(sp)
    800037a6:	7402                	ld	s0,32(sp)
    800037a8:	64e2                	ld	s1,24(sp)
    800037aa:	69a2                	ld	s3,8(sp)
    800037ac:	6145                	addi	sp,sp,48
    800037ae:	8082                	ret
    sprint("vfs_close: cannot close a directory!\n");
    800037b0:	00005517          	auipc	a0,0x5
    800037b4:	15050513          	addi	a0,a0,336 # 80008900 <rfs_i_ops+0x1888>
    800037b8:	43a010ef          	jal	80004bf2 <sprint>
    return -1;
    800037bc:	59fd                	li	s3,-1
    800037be:	b7d5                	j	800037a2 <vfs_close+0x3a>
      sprint("vfs_close: hook_close failed!\n");
    800037c0:	00005517          	auipc	a0,0x5
    800037c4:	16850513          	addi	a0,a0,360 # 80008928 <rfs_i_ops+0x18b0>
    800037c8:	42a010ef          	jal	80004bf2 <sprint>
    800037cc:	b7d1                	j	80003790 <vfs_close+0x28>
    hash_erase_dentry(dentry);
    800037ce:	8522                	mv	a0,s0
    800037d0:	937ff0ef          	jal	80003106 <hash_erase_dentry>
    free_vfs_dentry(dentry);
    800037d4:	8522                	mv	a0,s0
    800037d6:	8a5ff0ef          	jal	8000307a <free_vfs_dentry>
    inode->ref--;
    800037da:	40dc                	lw	a5,4(s1)
    800037dc:	37fd                	addiw	a5,a5,-1
    800037de:	0007871b          	sext.w	a4,a5
    800037e2:	c0dc                	sw	a5,4(s1)
    if (inode->ref == 0) {
    800037e4:	ff45                	bnez	a4,8000379c <vfs_close+0x34>
      if (viop_write_back_vinode(inode) != 0)
    800037e6:	68bc                	ld	a5,80(s1)
    800037e8:	6bbc                	ld	a5,80(a5)
    800037ea:	8526                	mv	a0,s1
    800037ec:	9782                	jalr	a5
    800037ee:	e901                	bnez	a0,800037fe <vfs_close+0x96>
      hash_erase_vinode(inode);
    800037f0:	8526                	mv	a0,s1
    800037f2:	e65ff0ef          	jal	80003656 <hash_erase_vinode>
      free_page(inode);
    800037f6:	8526                	mv	a0,s1
    800037f8:	f35fc0ef          	jal	8000072c <free_page>
    800037fc:	b745                	j	8000379c <vfs_close+0x34>
        panic("vfs_close: free inode failed!\n");
    800037fe:	00005517          	auipc	a0,0x5
    80003802:	14a50513          	addi	a0,a0,330 # 80008948 <rfs_i_ops+0x18d0>
    80003806:	490010ef          	jal	80004c96 <do_panic>

000000008000380a <default_alloc_vinode>:

//
// shared (default) actions on allocating a vfs inode.
//
struct vinode *default_alloc_vinode(struct super_block *sb) {
    8000380a:	1141                	addi	sp,sp,-16
    8000380c:	e406                	sd	ra,8(sp)
    8000380e:	e022                	sd	s0,0(sp)
    80003810:	842a                	mv	s0,a0
  struct vinode *vinode = (struct vinode *)alloc_page();
    80003812:	fcbfc0ef          	jal	800007dc <alloc_page>
  vinode->blocks = 0;
    80003816:	00052a23          	sw	zero,20(a0)
  vinode->inum = 0;
    8000381a:	00052023          	sw	zero,0(a0)
  vinode->nlinks = 0;
    8000381e:	00052823          	sw	zero,16(a0)
  vinode->ref = 0;
    80003822:	00052223          	sw	zero,4(a0)
  vinode->sb = sb;
    80003826:	e520                	sd	s0,72(a0)
  vinode->size = 0;
    80003828:	00052423          	sw	zero,8(a0)
  return vinode;
}
    8000382c:	60a2                	ld	ra,8(sp)
    8000382e:	6402                	ld	s0,0(sp)
    80003830:	0141                	addi	sp,sp,16
    80003832:	8082                	ret

0000000080003834 <prot_to_type>:

//
// convert permission code to permission types of PTE
//
uint64 prot_to_type(int prot, int user)
{
    80003834:	87aa                	mv	a5,a0
	uint64 perm = 0;
	if (prot & PROT_READ) perm |= PTE_R | PTE_A;
    80003836:	00157713          	andi	a4,a0,1
    8000383a:	ef19                	bnez	a4,80003858 <prot_to_type+0x24>
	uint64 perm = 0;
    8000383c:	4501                	li	a0,0
	if (prot & PROT_WRITE) perm |= PTE_W | PTE_D;
    8000383e:	0027f713          	andi	a4,a5,2
    80003842:	c319                	beqz	a4,80003848 <prot_to_type+0x14>
    80003844:	08456513          	ori	a0,a0,132
	if (prot & PROT_EXEC) perm |= PTE_X | PTE_A;
    80003848:	8b91                	andi	a5,a5,4
    8000384a:	cb91                	beqz	a5,8000385e <prot_to_type+0x2a>
    8000384c:	04856513          	ori	a0,a0,72
	if (perm == 0) perm = PTE_R;
	if (user) perm |= PTE_U;
    80003850:	c199                	beqz	a1,80003856 <prot_to_type+0x22>
    80003852:	01056513          	ori	a0,a0,16
	return perm;
}
    80003856:	8082                	ret
	if (prot & PROT_READ) perm |= PTE_R | PTE_A;
    80003858:	04200513          	li	a0,66
    8000385c:	b7cd                	j	8000383e <prot_to_type+0xa>
	if (perm == 0) perm = PTE_R;
    8000385e:	f96d                	bnez	a0,80003850 <prot_to_type+0x1c>
    80003860:	4509                	li	a0,2
    80003862:	b7fd                	j	80003850 <prot_to_type+0x1c>

0000000080003864 <page_walk>:
//
// traverse the page table (starting from page_dir) to find the corresponding pte of va.
// returns: PTE (page table entry) pointing to va.
//
pte_t* page_walk(pagetable_t page_dir, uint64 va, int alloc)
{
    80003864:	7179                	addi	sp,sp,-48
    80003866:	f406                	sd	ra,40(sp)
    80003868:	f022                	sd	s0,32(sp)
    8000386a:	ec26                	sd	s1,24(sp)
    8000386c:	e84a                	sd	s2,16(sp)
    8000386e:	e44e                	sd	s3,8(sp)
    80003870:	e052                	sd	s4,0(sp)
	if (va >= MAXVA) panic("page_walk");
    80003872:	57fd                	li	a5,-1
    80003874:	83e9                	srli	a5,a5,0x1a
    80003876:	00b7e763          	bltu	a5,a1,80003884 <page_walk+0x20>
    8000387a:	84aa                	mv	s1,a0
    8000387c:	892e                	mv	s2,a1
    8000387e:	8a32                	mv	s4,a2
	pagetable_t pt = page_dir;

	// traverse from page directory to page table.
	// as we use risc-v sv39 paging scheme, there will be 3 layers: page dir,
	// page medium dir, and page table.
	for (int level = 2; level > 0; level--) {
    80003880:	4989                	li	s3,2
    80003882:	a80d                	j	800038b4 <page_walk+0x50>
	if (va >= MAXVA) panic("page_walk");
    80003884:	00005517          	auipc	a0,0x5
    80003888:	0e450513          	addi	a0,a0,228 # 80008968 <rfs_i_ops+0x18f0>
    8000388c:	40a010ef          	jal	80004c96 <do_panic>
		if (*pte & PTE_V) {  //PTE valid
			// phisical address of pagetable of next level
			pt = (pagetable_t) PTE2PA(*pte);
		} else { //PTE invalid (not exist).
			// allocate a page (to be the new pagetable), if alloc == 1
			if (alloc && ((pt = (pte_t*) alloc_page(1)) != 0)) {
    80003890:	040a0663          	beqz	s4,800038dc <page_walk+0x78>
    80003894:	4505                	li	a0,1
    80003896:	f47fc0ef          	jal	800007dc <alloc_page>
    8000389a:	84aa                	mv	s1,a0
    8000389c:	c121                	beqz	a0,800038dc <page_walk+0x78>
				memset(pt, 0, PGSIZE);
    8000389e:	6605                	lui	a2,0x1
    800038a0:	4581                	li	a1,0
    800038a2:	0b3000ef          	jal	80004154 <memset>
				// writes the physical address of newly allocated page to pte, to establish the
				// page table tree.
				*pte = PA2PTE(pt) | PTE_V;
    800038a6:	00c4d793          	srli	a5,s1,0xc
    800038aa:	07aa                	slli	a5,a5,0xa
    800038ac:	0017e793          	ori	a5,a5,1
    800038b0:	e01c                	sd	a5,0(s0)
	for (int level = 2; level > 0; level--) {
    800038b2:	39fd                	addiw	s3,s3,-1
    800038b4:	03305663          	blez	s3,800038e0 <page_walk+0x7c>
		pte_t* pte = pt + PX(level, va);
    800038b8:	0039941b          	slliw	s0,s3,0x3
    800038bc:	0134043b          	addw	s0,s0,s3
    800038c0:	2431                	addiw	s0,s0,12
    800038c2:	00895433          	srl	s0,s2,s0
    800038c6:	1ff47413          	andi	s0,s0,511
    800038ca:	040e                	slli	s0,s0,0x3
    800038cc:	9426                	add	s0,s0,s1
		if (*pte & PTE_V) {  //PTE valid
    800038ce:	6004                	ld	s1,0(s0)
    800038d0:	0014f793          	andi	a5,s1,1
    800038d4:	dfd5                	beqz	a5,80003890 <page_walk+0x2c>
			pt = (pagetable_t) PTE2PA(*pte);
    800038d6:	80a9                	srli	s1,s1,0xa
    800038d8:	04b2                	slli	s1,s1,0xc
    800038da:	bfe1                	j	800038b2 <page_walk+0x4e>
			} else //returns NULL, if alloc == 0, or no more physical page remains
				return 0;
    800038dc:	4501                	li	a0,0
    800038de:	a039                	j	800038ec <page_walk+0x88>
		}
	}

	// return a PTE which contains phisical address of a page
	return pt + PX(0, va);
    800038e0:	00c95513          	srli	a0,s2,0xc
    800038e4:	1ff57513          	andi	a0,a0,511
    800038e8:	050e                	slli	a0,a0,0x3
    800038ea:	9526                	add	a0,a0,s1
}
    800038ec:	70a2                	ld	ra,40(sp)
    800038ee:	7402                	ld	s0,32(sp)
    800038f0:	64e2                	ld	s1,24(sp)
    800038f2:	6942                	ld	s2,16(sp)
    800038f4:	69a2                	ld	s3,8(sp)
    800038f6:	6a02                	ld	s4,0(sp)
    800038f8:	6145                	addi	sp,sp,48
    800038fa:	8082                	ret

00000000800038fc <map_pages>:
{
    800038fc:	7179                	addi	sp,sp,-48
    800038fe:	f406                	sd	ra,40(sp)
    80003900:	f022                	sd	s0,32(sp)
    80003902:	ec26                	sd	s1,24(sp)
    80003904:	e84a                	sd	s2,16(sp)
    80003906:	e44e                	sd	s3,8(sp)
    80003908:	e052                	sd	s4,0(sp)
    8000390a:	89aa                	mv	s3,a0
    8000390c:	84b6                	mv	s1,a3
    8000390e:	8a3a                	mv	s4,a4
	for (first = ROUNDDOWN(va, PGSIZE), last = ROUNDDOWN(va + size - 1, PGSIZE);
    80003910:	77fd                	lui	a5,0xfffff
    80003912:	00f5f433          	and	s0,a1,a5
    80003916:	00c58933          	add	s2,a1,a2
    8000391a:	197d                	addi	s2,s2,-1
    8000391c:	00f97933          	and	s2,s2,a5
		 first <= last; first += PGSIZE, pa += PGSIZE) {
    80003920:	02896f63          	bltu	s2,s0,8000395e <map_pages+0x62>
		if ((pte = page_walk(page_dir, first, 1)) == 0) return -1;
    80003924:	4605                	li	a2,1
    80003926:	85a2                	mv	a1,s0
    80003928:	854e                	mv	a0,s3
    8000392a:	f3bff0ef          	jal	80003864 <page_walk>
    8000392e:	c129                	beqz	a0,80003970 <map_pages+0x74>
		if (*pte & PTE_V)
    80003930:	611c                	ld	a5,0(a0)
    80003932:	8b85                	andi	a5,a5,1
    80003934:	ef89                	bnez	a5,8000394e <map_pages+0x52>
		*pte = PA2PTE(pa) | perm | PTE_V;
    80003936:	00c4d793          	srli	a5,s1,0xc
    8000393a:	07aa                	slli	a5,a5,0xa
    8000393c:	00fa67b3          	or	a5,s4,a5
    80003940:	0017e793          	ori	a5,a5,1
    80003944:	e11c                	sd	a5,0(a0)
		 first <= last; first += PGSIZE, pa += PGSIZE) {
    80003946:	6785                	lui	a5,0x1
    80003948:	943e                	add	s0,s0,a5
    8000394a:	94be                	add	s1,s1,a5
    8000394c:	bfd1                	j	80003920 <map_pages+0x24>
			panic("map_pages fails on mapping va (0x%lx) to pa (0x%lx)", first, pa);
    8000394e:	8626                	mv	a2,s1
    80003950:	85a2                	mv	a1,s0
    80003952:	00005517          	auipc	a0,0x5
    80003956:	02650513          	addi	a0,a0,38 # 80008978 <rfs_i_ops+0x1900>
    8000395a:	33c010ef          	jal	80004c96 <do_panic>
	return 0;
    8000395e:	4501                	li	a0,0
}
    80003960:	70a2                	ld	ra,40(sp)
    80003962:	7402                	ld	s0,32(sp)
    80003964:	64e2                	ld	s1,24(sp)
    80003966:	6942                	ld	s2,16(sp)
    80003968:	69a2                	ld	s3,8(sp)
    8000396a:	6a02                	ld	s4,0(sp)
    8000396c:	6145                	addi	sp,sp,48
    8000396e:	8082                	ret
		if ((pte = page_walk(page_dir, first, 1)) == 0) return -1;
    80003970:	557d                	li	a0,-1
    80003972:	b7fd                	j	80003960 <map_pages+0x64>

0000000080003974 <lookup_pa>:
uint64 lookup_pa(pagetable_t pagetable, uint64 va)
{
	pte_t* pte;
	uint64 pa;

	if (va >= MAXVA) return 0;
    80003974:	57fd                	li	a5,-1
    80003976:	83e9                	srli	a5,a5,0x1a
    80003978:	00b7f463          	bgeu	a5,a1,80003980 <lookup_pa+0xc>
    8000397c:	4501                	li	a0,0
	if (pte == 0 || (*pte & PTE_V) == 0 || ((*pte & PTE_R) == 0 && (*pte & PTE_W) == 0))
		return 0;
	pa = PTE2PA(*pte);

	return pa;
}
    8000397e:	8082                	ret
{
    80003980:	1141                	addi	sp,sp,-16
    80003982:	e406                	sd	ra,8(sp)
	pte = page_walk(pagetable, va, 0);
    80003984:	4601                	li	a2,0
    80003986:	edfff0ef          	jal	80003864 <page_walk>
	if (pte == 0 || (*pte & PTE_V) == 0 || ((*pte & PTE_R) == 0 && (*pte & PTE_W) == 0))
    8000398a:	cd11                	beqz	a0,800039a6 <lookup_pa+0x32>
    8000398c:	611c                	ld	a5,0(a0)
    8000398e:	0017f513          	andi	a0,a5,1
    80003992:	c519                	beqz	a0,800039a0 <lookup_pa+0x2c>
    80003994:	0067f513          	andi	a0,a5,6
    80003998:	c501                	beqz	a0,800039a0 <lookup_pa+0x2c>
	pa = PTE2PA(*pte);
    8000399a:	83a9                	srli	a5,a5,0xa
    8000399c:	00c79513          	slli	a0,a5,0xc
}
    800039a0:	60a2                	ld	ra,8(sp)
    800039a2:	0141                	addi	sp,sp,16
    800039a4:	8082                	ret
		return 0;
    800039a6:	4501                	li	a0,0
    800039a8:	bfe5                	j	800039a0 <lookup_pa+0x2c>

00000000800039aa <kern_vm_map>:

//
// maps virtual address [va, va+sz] to [pa, pa+sz] (for kernel).
//
void kern_vm_map(pagetable_t page_dir, uint64 va, uint64 pa, uint64 sz, int perm)
{
    800039aa:	1141                	addi	sp,sp,-16
    800039ac:	e406                	sd	ra,8(sp)
    800039ae:	87b6                	mv	a5,a3
	// map_pages is defined in kernel/vmm.c
	if (map_pages(page_dir, va, sz, pa, perm) != 0) panic("kern_vm_map");
    800039b0:	86b2                	mv	a3,a2
    800039b2:	863e                	mv	a2,a5
    800039b4:	f49ff0ef          	jal	800038fc <map_pages>
    800039b8:	e501                	bnez	a0,800039c0 <kern_vm_map+0x16>
}
    800039ba:	60a2                	ld	ra,8(sp)
    800039bc:	0141                	addi	sp,sp,16
    800039be:	8082                	ret
	if (map_pages(page_dir, va, sz, pa, perm) != 0) panic("kern_vm_map");
    800039c0:	00005517          	auipc	a0,0x5
    800039c4:	ff050513          	addi	a0,a0,-16 # 800089b0 <rfs_i_ops+0x1938>
    800039c8:	2ce010ef          	jal	80004c96 <do_panic>

00000000800039cc <kern_vm_init>:

//
// kern_vm_init() constructs the kernel page table.
//
void kern_vm_init(void)
{
    800039cc:	1101                	addi	sp,sp,-32
    800039ce:	ec06                	sd	ra,24(sp)
    800039d0:	e822                	sd	s0,16(sp)
    800039d2:	e426                	sd	s1,8(sp)
	// pagetable_t is defined in kernel/riscv.h. it's actually uint64*
	pagetable_t t_page_dir;

	// allocate a page (t_page_dir) to be the page directory for kernel. alloc_page is defined in kernel/pmm.c
	t_page_dir = (pagetable_t) alloc_page();
    800039d4:	e09fc0ef          	jal	800007dc <alloc_page>
    800039d8:	842a                	mv	s0,a0
	// memset is defined in util/string.c
	memset(t_page_dir, 0, PGSIZE);
    800039da:	6605                	lui	a2,0x1
    800039dc:	4581                	li	a1,0
    800039de:	776000ef          	jal	80004154 <memset>

	// map virtual address [KERN_BASE, _etext] to physical address [DRAM_BASE, DRAM_BASE+(_etext - KERN_BASE)],
	// to maintain (direct) text section kernel address mapping.
	kern_vm_map(t_page_dir, KERN_BASE, DRAM_BASE, (uint64)
    800039e2:	00005497          	auipc	s1,0x5
    800039e6:	61e48493          	addi	s1,s1,1566 # 80009000 <fromhost>
	_etext - KERN_BASE,
		prot_to_type(PROT_READ | PROT_EXEC, 0));
    800039ea:	4581                	li	a1,0
    800039ec:	4515                	li	a0,5
    800039ee:	e47ff0ef          	jal	80003834 <prot_to_type>
	kern_vm_map(t_page_dir, KERN_BASE, DRAM_BASE, (uint64)
    800039f2:	0005071b          	sext.w	a4,a0
    800039f6:	80005697          	auipc	a3,0x80005
    800039fa:	60a68693          	addi	a3,a3,1546 # 9000 <elf_fpread-0x7fff7000>
    800039fe:	4605                	li	a2,1
    80003a00:	067e                	slli	a2,a2,0x1f
    80003a02:	85b2                	mv	a1,a2
    80003a04:	8522                	mv	a0,s0
    80003a06:	fa5ff0ef          	jal	800039aa <kern_vm_map>

	sprint("KERN_BASE 0x%lx\n", lookup_pa(t_page_dir, KERN_BASE));
    80003a0a:	4585                	li	a1,1
    80003a0c:	05fe                	slli	a1,a1,0x1f
    80003a0e:	8522                	mv	a0,s0
    80003a10:	f65ff0ef          	jal	80003974 <lookup_pa>
    80003a14:	85aa                	mv	a1,a0
    80003a16:	00005517          	auipc	a0,0x5
    80003a1a:	faa50513          	addi	a0,a0,-86 # 800089c0 <rfs_i_ops+0x1948>
    80003a1e:	1d4010ef          	jal	80004bf2 <sprint>
	// without copying pages between kernel and user spaces.
	kern_vm_map(t_page_dir, (uint64)
	_etext, (uint64)
	_etext, PHYS_TOP - (uint64)
	_etext,
		prot_to_type(PROT_READ | PROT_WRITE, 0));
    80003a22:	4581                	li	a1,0
    80003a24:	450d                	li	a0,3
    80003a26:	e0fff0ef          	jal	80003834 <prot_to_type>
	kern_vm_map(t_page_dir, (uint64)
    80003a2a:	46c5                	li	a3,17
    80003a2c:	06ee                	slli	a3,a3,0x1b
    80003a2e:	0005071b          	sext.w	a4,a0
    80003a32:	8e85                	sub	a3,a3,s1
    80003a34:	8626                	mv	a2,s1
    80003a36:	85a6                	mv	a1,s1
    80003a38:	8522                	mv	a0,s0
    80003a3a:	f71ff0ef          	jal	800039aa <kern_vm_map>

	sprint("physical address of _etext is: 0x%lx\n", lookup_pa(t_page_dir, (uint64)
    80003a3e:	85a6                	mv	a1,s1
    80003a40:	8522                	mv	a0,s0
    80003a42:	f33ff0ef          	jal	80003974 <lookup_pa>
    80003a46:	85aa                	mv	a1,a0
    80003a48:	00005517          	auipc	a0,0x5
    80003a4c:	f9050513          	addi	a0,a0,-112 # 800089d8 <rfs_i_ops+0x1960>
    80003a50:	1a2010ef          	jal	80004bf2 <sprint>
	_etext));

	g_kernel_pagetable = t_page_dir;
    80003a54:	0000c797          	auipc	a5,0xc
    80003a58:	6e87ba23          	sd	s0,1780(a5) # 80010148 <g_kernel_pagetable>
}
    80003a5c:	60e2                	ld	ra,24(sp)
    80003a5e:	6442                	ld	s0,16(sp)
    80003a60:	64a2                	ld	s1,8(sp)
    80003a62:	6105                	addi	sp,sp,32
    80003a64:	8082                	ret

0000000080003a66 <user_va_to_pa>:
//
// convert and return the corresponding physical address of a virtual address (va) of
// application.
//
void* user_va_to_pa(pagetable_t page_dir, void* va)
{
    80003a66:	1141                	addi	sp,sp,-16
    80003a68:	e406                	sd	ra,8(sp)
    80003a6a:	e022                	sd	s0,0(sp)
    80003a6c:	842e                	mv	s0,a1
	// Here, PYHS_ADDR() means retrieving the starting address (4KB aligned), and
	// (va & (1<<PGSHIFT -1)) means computing the offset of "va" inside its page.
	// Also, it is possible that "va" is not mapped at all. in such case, we can find
	// invalid PTE, and should return NULL.
	uint64 pa;
	pa = lookup_pa(page_dir, (uint64)
    80003a6e:	f07ff0ef          	jal	80003974 <lookup_pa>
	va);
	pa += ((uint64)
	va & ((1 << PGSHIFT) - 1));
    80003a72:	6785                	lui	a5,0x1
    80003a74:	17fd                	addi	a5,a5,-1 # fff <elf_fpread-0x7ffff001>
    80003a76:	8c7d                	and	s0,s0,a5
	return (void*) pa;

}
    80003a78:	9522                	add	a0,a0,s0
    80003a7a:	60a2                	ld	ra,8(sp)
    80003a7c:	6402                	ld	s0,0(sp)
    80003a7e:	0141                	addi	sp,sp,16
    80003a80:	8082                	ret

0000000080003a82 <user_vm_map>:

//
// maps virtual address [va, va+sz] to [pa, pa+sz] (for user application).
//
void user_vm_map(pagetable_t page_dir, uint64 va, uint64 size, uint64 pa, int perm)
{
    80003a82:	1141                	addi	sp,sp,-16
    80003a84:	e406                	sd	ra,8(sp)
	if (map_pages(page_dir, va, size, pa, perm) != 0) {
    80003a86:	e77ff0ef          	jal	800038fc <map_pages>
    80003a8a:	e501                	bnez	a0,80003a92 <user_vm_map+0x10>
		panic("fail to user_vm_map .\n");
	}
}
    80003a8c:	60a2                	ld	ra,8(sp)
    80003a8e:	0141                	addi	sp,sp,16
    80003a90:	8082                	ret
		panic("fail to user_vm_map .\n");
    80003a92:	00005517          	auipc	a0,0x5
    80003a96:	f6e50513          	addi	a0,a0,-146 # 80008a00 <rfs_i_ops+0x1988>
    80003a9a:	1fc010ef          	jal	80004c96 <do_panic>

0000000080003a9e <user_vm_unmap>:

//
// unmap virtual address [va, va+size] from the user app.
// reclaim the physical pages if free!=0
//
void user_vm_unmap(pagetable_t page_dir, uint64 va, uint64 size, int free) {
    80003a9e:	1141                	addi	sp,sp,-16
    80003aa0:	e406                	sd	ra,8(sp)
    80003aa2:	e022                	sd	s0,0(sp)
  // basic idea here is to first locate the PTEs of the virtual pages, and then reclaim
  // (use free_page() defined in pmm.c) the physical pages. lastly, invalidate the PTEs.
  // as naive_free reclaims only one page at a time, you only need to consider one page
  // to make user/app_naive_malloc to behave correctly.
	pte_t* pte;
	pte = page_walk(page_dir, va, 0);
    80003aa4:	4601                	li	a2,0
    80003aa6:	dbfff0ef          	jal	80003864 <page_walk>
	if (pte == 0) {
    80003aaa:	cd11                	beqz	a0,80003ac6 <user_vm_unmap+0x28>
    80003aac:	842a                	mv	s0,a0
		panic("fail to user_vm_unmap .\n");
	}
	free_page((void*) PTE2PA(*pte));
    80003aae:	6108                	ld	a0,0(a0)
    80003ab0:	8129                	srli	a0,a0,0xa
    80003ab2:	0532                	slli	a0,a0,0xc
    80003ab4:	c79fc0ef          	jal	8000072c <free_page>
	*pte &= ~PTE_V;
    80003ab8:	601c                	ld	a5,0(s0)
    80003aba:	9bf9                	andi	a5,a5,-2
    80003abc:	e01c                	sd	a5,0(s0)

}
    80003abe:	60a2                	ld	ra,8(sp)
    80003ac0:	6402                	ld	s0,0(sp)
    80003ac2:	0141                	addi	sp,sp,16
    80003ac4:	8082                	ret
		panic("fail to user_vm_unmap .\n");
    80003ac6:	00005517          	auipc	a0,0x5
    80003aca:	f5250513          	addi	a0,a0,-174 # 80008a18 <rfs_i_ops+0x19a0>
    80003ace:	1c8010ef          	jal	80004c96 <do_panic>

0000000080003ad2 <print_proc_vmspace>:

//
// debug function, print the vm space of a process. added @lab3_1
//
void print_proc_vmspace(process* proc) {
    80003ad2:	1101                	addi	sp,sp,-32
    80003ad4:	ec06                	sd	ra,24(sp)
    80003ad6:	e822                	sd	s0,16(sp)
    80003ad8:	e426                	sd	s1,8(sp)
    80003ada:	e04a                	sd	s2,0(sp)
    80003adc:	842a                	mv	s0,a0
  sprint( "======\tbelow is the vm space of process%d\t========\n", proc->pid );
    80003ade:	14053583          	ld	a1,320(a0)
    80003ae2:	00005517          	auipc	a0,0x5
    80003ae6:	f5650513          	addi	a0,a0,-170 # 80008a38 <rfs_i_ops+0x19c0>
    80003aea:	108010ef          	jal	80004bf2 <sprint>
  for( int i=0; i<proc->total_mapped_region; i++ ){
    80003aee:	4481                	li	s1,0
    80003af0:	a02d                	j	80003b1a <print_proc_vmspace+0x48>
    sprint( "-va:%lx, npage:%d, ", proc->mapped_info[i].va, proc->mapped_info[i].npages);
    switch(proc->mapped_info[i].seg_type){
      case CODE_SEGMENT: sprint( "type: CODE SEGMENT" ); break;
    80003af2:	00005517          	auipc	a0,0x5
    80003af6:	f9650513          	addi	a0,a0,-106 # 80008a88 <rfs_i_ops+0x1a10>
    80003afa:	0f8010ef          	jal	80004bf2 <sprint>
      case DATA_SEGMENT: sprint( "type: DATA SEGMENT" ); break;
      case STACK_SEGMENT: sprint( "type: STACK SEGMENT" ); break;
      case CONTEXT_SEGMENT: sprint( "type: TRAPFRAME SEGMENT" ); break;
      case SYSTEM_SEGMENT: sprint( "type: USER KERNEL STACK SEGMENT" ); break;
    }
    sprint( ", mapped to pa:%lx\n", lookup_pa(proc->pagetable, proc->mapped_info[i].va) );
    80003afe:	6c1c                	ld	a5,24(s0)
    80003b00:	97ca                	add	a5,a5,s2
    80003b02:	638c                	ld	a1,0(a5)
    80003b04:	6408                	ld	a0,8(s0)
    80003b06:	e6fff0ef          	jal	80003974 <lookup_pa>
    80003b0a:	85aa                	mv	a1,a0
    80003b0c:	00005517          	auipc	a0,0x5
    80003b10:	ffc50513          	addi	a0,a0,-4 # 80008b08 <rfs_i_ops+0x1a90>
    80003b14:	0de010ef          	jal	80004bf2 <sprint>
  for( int i=0; i<proc->total_mapped_region; i++ ){
    80003b18:	2485                	addiw	s1,s1,1
    80003b1a:	501c                	lw	a5,32(s0)
    80003b1c:	06f4d963          	bge	s1,a5,80003b8e <print_proc_vmspace+0xbc>
    sprint( "-va:%lx, npage:%d, ", proc->mapped_info[i].va, proc->mapped_info[i].npages);
    80003b20:	6c1c                	ld	a5,24(s0)
    80003b22:	00449913          	slli	s2,s1,0x4
    80003b26:	97ca                	add	a5,a5,s2
    80003b28:	4790                	lw	a2,8(a5)
    80003b2a:	638c                	ld	a1,0(a5)
    80003b2c:	00005517          	auipc	a0,0x5
    80003b30:	f4450513          	addi	a0,a0,-188 # 80008a70 <rfs_i_ops+0x19f8>
    80003b34:	0be010ef          	jal	80004bf2 <sprint>
    switch(proc->mapped_info[i].seg_type){
    80003b38:	6c1c                	ld	a5,24(s0)
    80003b3a:	97ca                	add	a5,a5,s2
    80003b3c:	47dc                	lw	a5,12(a5)
    80003b3e:	4715                	li	a4,5
    80003b40:	faf76fe3          	bltu	a4,a5,80003afe <print_proc_vmspace+0x2c>
    80003b44:	078a                	slli	a5,a5,0x2
    80003b46:	00003717          	auipc	a4,0x3
    80003b4a:	62670713          	addi	a4,a4,1574 # 8000716c <rfs_i_ops+0xf4>
    80003b4e:	97ba                	add	a5,a5,a4
    80003b50:	439c                	lw	a5,0(a5)
    80003b52:	97ba                	add	a5,a5,a4
    80003b54:	8782                	jr	a5
      case DATA_SEGMENT: sprint( "type: DATA SEGMENT" ); break;
    80003b56:	00005517          	auipc	a0,0x5
    80003b5a:	f4a50513          	addi	a0,a0,-182 # 80008aa0 <rfs_i_ops+0x1a28>
    80003b5e:	094010ef          	jal	80004bf2 <sprint>
    80003b62:	bf71                	j	80003afe <print_proc_vmspace+0x2c>
      case STACK_SEGMENT: sprint( "type: STACK SEGMENT" ); break;
    80003b64:	00005517          	auipc	a0,0x5
    80003b68:	f5450513          	addi	a0,a0,-172 # 80008ab8 <rfs_i_ops+0x1a40>
    80003b6c:	086010ef          	jal	80004bf2 <sprint>
    80003b70:	b779                	j	80003afe <print_proc_vmspace+0x2c>
      case CONTEXT_SEGMENT: sprint( "type: TRAPFRAME SEGMENT" ); break;
    80003b72:	00005517          	auipc	a0,0x5
    80003b76:	f5e50513          	addi	a0,a0,-162 # 80008ad0 <rfs_i_ops+0x1a58>
    80003b7a:	078010ef          	jal	80004bf2 <sprint>
    80003b7e:	b741                	j	80003afe <print_proc_vmspace+0x2c>
      case SYSTEM_SEGMENT: sprint( "type: USER KERNEL STACK SEGMENT" ); break;
    80003b80:	00005517          	auipc	a0,0x5
    80003b84:	f6850513          	addi	a0,a0,-152 # 80008ae8 <rfs_i_ops+0x1a70>
    80003b88:	06a010ef          	jal	80004bf2 <sprint>
    80003b8c:	bf8d                	j	80003afe <print_proc_vmspace+0x2c>
  }
}
    80003b8e:	60e2                	ld	ra,24(sp)
    80003b90:	6442                	ld	s0,16(sp)
    80003b92:	64a2                	ld	s1,8(sp)
    80003b94:	6902                	ld	s2,0(sp)
    80003b96:	6105                	addi	sp,sp,32
    80003b98:	8082                	ret

0000000080003b9a <delegate_traps>:

//
// delegate (almost all) interrupts and most exceptions to S-mode.
// after delegation, syscalls will handled by the PKE OS kernel running in S-mode.
//
static void delegate_traps() {
    80003b9a:	1141                	addi	sp,sp,-16
    80003b9c:	e406                	sd	ra,8(sp)
    asm("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp;                               \
  })

static inline int supports_extension(char ext) {
  return read_const_csr(misa) & (1 << (ext - 'A'));
    80003b9e:	301027f3          	csrr	a5,misa
    80003ba2:	00040737          	lui	a4,0x40
    80003ba6:	8ff9                	and	a5,a5,a4
    80003ba8:	2781                	sext.w	a5,a5
  // supports_extension macro is defined in kernel/riscv.h
  if (!supports_extension('S')) {
    80003baa:	cb85                	beqz	a5,80003bda <delegate_traps+0x40>

  // writes 64-bit values (interrupts and exceptions) to 'mideleg' and 'medeleg' (two
  // priviledged registers of RV64G machine) respectively.
  //
  // write_csr and read_csr are macros defined in kernel/riscv.h
  write_csr(mideleg, interrupts);
    80003bac:	22200713          	li	a4,546
    80003bb0:	30371073          	csrw	mideleg,a4
  write_csr(medeleg, exceptions);
    80003bb4:	67ad                	lui	a5,0xb
    80003bb6:	10978793          	addi	a5,a5,265 # b109 <elf_fpread-0x7fff4ef7>
    80003bba:	30279073          	csrw	medeleg,a5
  assert(read_csr(mideleg) == interrupts);
    80003bbe:	303027f3          	csrr	a5,mideleg
    80003bc2:	02e79363          	bne	a5,a4,80003be8 <delegate_traps+0x4e>
  assert(read_csr(medeleg) == exceptions);
    80003bc6:	30202773          	csrr	a4,medeleg
    80003bca:	67ad                	lui	a5,0xb
    80003bcc:	10978793          	addi	a5,a5,265 # b109 <elf_fpread-0x7fff4ef7>
    80003bd0:	04f71063          	bne	a4,a5,80003c10 <delegate_traps+0x76>
}
    80003bd4:	60a2                	ld	ra,8(sp)
    80003bd6:	0141                	addi	sp,sp,16
    80003bd8:	8082                	ret
    sprint("S mode is not supported.\n");
    80003bda:	00005517          	auipc	a0,0x5
    80003bde:	f4650513          	addi	a0,a0,-186 # 80008b20 <rfs_i_ops+0x1aa8>
    80003be2:	010010ef          	jal	80004bf2 <sprint>
    return;
    80003be6:	b7fd                	j	80003bd4 <delegate_traps+0x3a>
  assert(read_csr(mideleg) == interrupts);
    80003be8:	00005697          	auipc	a3,0x5
    80003bec:	f5868693          	addi	a3,a3,-168 # 80008b40 <rfs_i_ops+0x1ac8>
    80003bf0:	04b00613          	li	a2,75
    80003bf4:	00005597          	auipc	a1,0x5
    80003bf8:	f6c58593          	addi	a1,a1,-148 # 80008b60 <rfs_i_ops+0x1ae8>
    80003bfc:	00004517          	auipc	a0,0x4
    80003c00:	b8c50513          	addi	a0,a0,-1140 # 80007788 <rfs_i_ops+0x710>
    80003c04:	7ef000ef          	jal	80004bf2 <sprint>
    80003c08:	6541                	lui	a0,0x10
    80003c0a:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    80003c0c:	006010ef          	jal	80004c12 <poweroff>
  assert(read_csr(medeleg) == exceptions);
    80003c10:	00005697          	auipc	a3,0x5
    80003c14:	f6868693          	addi	a3,a3,-152 # 80008b78 <rfs_i_ops+0x1b00>
    80003c18:	04c00613          	li	a2,76
    80003c1c:	00005597          	auipc	a1,0x5
    80003c20:	f4458593          	addi	a1,a1,-188 # 80008b60 <rfs_i_ops+0x1ae8>
    80003c24:	00004517          	auipc	a0,0x4
    80003c28:	b6450513          	addi	a0,a0,-1180 # 80007788 <rfs_i_ops+0x710>
    80003c2c:	7c7000ef          	jal	80004bf2 <sprint>
    80003c30:	6541                	lui	a0,0x10
    80003c32:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    80003c34:	7df000ef          	jal	80004c12 <poweroff>

0000000080003c38 <init_dtb>:
void init_dtb(uint64 dtb) {
    80003c38:	1141                	addi	sp,sp,-16
    80003c3a:	e406                	sd	ra,8(sp)
    80003c3c:	e022                	sd	s0,0(sp)
    80003c3e:	842a                	mv	s0,a0
  query_htif(dtb);
    80003c40:	3d1000ef          	jal	80004810 <query_htif>
  if (htif) sprint("HTIF is available!\r\n");
    80003c44:	0000c797          	auipc	a5,0xc
    80003c48:	51c7b783          	ld	a5,1308(a5) # 80010160 <htif>
    80003c4c:	e39d                	bnez	a5,80003c72 <init_dtb+0x3a>
  query_mem(dtb);
    80003c4e:	8522                	mv	a0,s0
    80003c50:	601000ef          	jal	80004a50 <query_mem>
  sprint("(Emulated) memory size: %ld MB\n", g_mem_size >> 20);
    80003c54:	0000c597          	auipc	a1,0xc
    80003c58:	5145b583          	ld	a1,1300(a1) # 80010168 <g_mem_size>
    80003c5c:	81d1                	srli	a1,a1,0x14
    80003c5e:	00005517          	auipc	a0,0x5
    80003c62:	f5250513          	addi	a0,a0,-174 # 80008bb0 <rfs_i_ops+0x1b38>
    80003c66:	78d000ef          	jal	80004bf2 <sprint>
}
    80003c6a:	60a2                	ld	ra,8(sp)
    80003c6c:	6402                	ld	s0,0(sp)
    80003c6e:	0141                	addi	sp,sp,16
    80003c70:	8082                	ret
  if (htif) sprint("HTIF is available!\r\n");
    80003c72:	00005517          	auipc	a0,0x5
    80003c76:	f2650513          	addi	a0,a0,-218 # 80008b98 <rfs_i_ops+0x1b20>
    80003c7a:	779000ef          	jal	80004bf2 <sprint>
    80003c7e:	bfc1                	j	80003c4e <init_dtb+0x16>

0000000080003c80 <timerinit>:
//
// enabling timer interrupt (irq) in Machine mode. added @lab1_3
//
void timerinit(uintptr_t hartid) {
  // fire timer irq after TIMER_INTERVAL from now.
  *(uint64*)CLINT_MTIMECMP(hartid) = *(uint64*)CLINT_MTIME + TIMER_INTERVAL;
    80003c80:	0200c7b7          	lui	a5,0x200c
    80003c84:	17e1                	addi	a5,a5,-8 # 200bff8 <elf_fpread-0x7dff4008>
    80003c86:	639c                	ld	a5,0(a5)
    80003c88:	00401737          	lui	a4,0x401
    80003c8c:	80070713          	addi	a4,a4,-2048 # 400800 <elf_fpread-0x7fbff800>
    80003c90:	953a                	add	a0,a0,a4
    80003c92:	050e                	slli	a0,a0,0x3
    80003c94:	000f4737          	lui	a4,0xf4
    80003c98:	24070713          	addi	a4,a4,576 # f4240 <elf_fpread-0x7ff0bdc0>
    80003c9c:	97ba                	add	a5,a5,a4
    80003c9e:	e11c                	sd	a5,0(a0)

  // enable machine-mode timer irq in MIE (Machine Interrupt Enable) csr.
  write_csr(mie, read_csr(mie) | MIE_MTIE);
    80003ca0:	304027f3          	csrr	a5,mie
    80003ca4:	0807e793          	ori	a5,a5,128
    80003ca8:	30479073          	csrw	mie,a5
}
    80003cac:	8082                	ret

0000000080003cae <m_start>:

//
// m_start: machine mode C entry point.
//
void m_start(uintptr_t hartid, uintptr_t dtb) {
    80003cae:	1101                	addi	sp,sp,-32
    80003cb0:	ec06                	sd	ra,24(sp)
    80003cb2:	e822                	sd	s0,16(sp)
    80003cb4:	e426                	sd	s1,8(sp)
    80003cb6:	842a                	mv	s0,a0
    80003cb8:	84ae                	mv	s1,a1
  // init the spike file interface (stdin,stdout,stderr)
  // functions with "spike_" prefix are all defined in codes under spike_interface/,
  // sprint is also defined in spike_interface/spike_utils.c
  spike_file_init();
    80003cba:	069000ef          	jal	80004522 <spike_file_init>
  sprint("In m_start, hartid:%d\n", hartid);
    80003cbe:	85a2                	mv	a1,s0
    80003cc0:	00005517          	auipc	a0,0x5
    80003cc4:	f1050513          	addi	a0,a0,-240 # 80008bd0 <rfs_i_ops+0x1b58>
    80003cc8:	72b000ef          	jal	80004bf2 <sprint>

  // init HTIF (Host-Target InterFace) and memory by using the Device Table Blob (DTB)
  // init_dtb() is defined above.
  init_dtb(dtb);
    80003ccc:	8526                	mv	a0,s1
    80003cce:	f6bff0ef          	jal	80003c38 <init_dtb>

  // save the address of trap frame for interrupt in M mode to "mscratch". added @lab1_2
  write_csr(mscratch, &g_itrframe);
    80003cd2:	0000b797          	auipc	a5,0xb
    80003cd6:	ece78793          	addi	a5,a5,-306 # 8000eba0 <g_itrframe>
    80003cda:	34079073          	csrw	mscratch,a5

  // set previous privilege mode to S (Supervisor), and will enter S mode after 'mret'
  // write_csr is a macro defined in kernel/riscv.h
  write_csr(mstatus, ((read_csr(mstatus) & ~MSTATUS_MPP_MASK) | MSTATUS_MPP_S));
    80003cde:	300027f3          	csrr	a5,mstatus
    80003ce2:	7779                	lui	a4,0xffffe
    80003ce4:	7ff70713          	addi	a4,a4,2047 # ffffffffffffe7ff <_end+0xffffffff7ffed7ff>
    80003ce8:	8ff9                	and	a5,a5,a4
    80003cea:	6705                	lui	a4,0x1
    80003cec:	80070713          	addi	a4,a4,-2048 # 800 <elf_fpread-0x7ffff800>
    80003cf0:	8fd9                	or	a5,a5,a4
    80003cf2:	30079073          	csrw	mstatus,a5

  // set M Exception Program Counter to sstart, for mret (requires gcc -mcmodel=medany)
  write_csr(mepc, (uint64)s_start);
    80003cf6:	ffffd797          	auipc	a5,0xffffd
    80003cfa:	9e278793          	addi	a5,a5,-1566 # 800006d8 <s_start>
    80003cfe:	34179073          	csrw	mepc,a5

  // setup trap handling vector for machine mode. added @lab1_2
  write_csr(mtvec, (uint64)mtrapvec);
    80003d02:	00000797          	auipc	a5,0x0
    80003d06:	15e78793          	addi	a5,a5,350 # 80003e60 <mtrapvec>
    80003d0a:	30579073          	csrw	mtvec,a5

  // enable machine-mode interrupts. added @lab1_3
  write_csr(mstatus, read_csr(mstatus) | MSTATUS_MIE);
    80003d0e:	300027f3          	csrr	a5,mstatus
    80003d12:	0087e793          	ori	a5,a5,8
    80003d16:	30079073          	csrw	mstatus,a5

  // delegate all interrupts and exceptions to supervisor mode.
  // delegate_traps() is defined above.
  delegate_traps();
    80003d1a:	e81ff0ef          	jal	80003b9a <delegate_traps>

  // also enables interrupt handling in supervisor mode. added @lab1_3
  write_csr(sie, read_csr(sie) | SIE_SEIE | SIE_STIE | SIE_SSIE);
    80003d1e:	104027f3          	csrr	a5,sie
    80003d22:	2227e793          	ori	a5,a5,546
    80003d26:	10479073          	csrw	sie,a5

  // init timing. added @lab1_3
  timerinit(hartid);
    80003d2a:	8522                	mv	a0,s0
    80003d2c:	f55ff0ef          	jal	80003c80 <timerinit>

  // switch to supervisor mode (S mode) and jump to s_start(), i.e., set pc to mepc
  asm volatile("mret");
    80003d30:	30200073          	mret
}
    80003d34:	60e2                	ld	ra,24(sp)
    80003d36:	6442                	ld	s0,16(sp)
    80003d38:	64a2                	ld	s1,8(sp)
    80003d3a:	6105                	addi	sp,sp,32
    80003d3c:	8082                	ret

0000000080003d3e <handle_timer>:

// added @lab1_3
static void handle_timer() {
  int cpuid = 0;
  // setup the timer fired at next time (TIMER_INTERVAL from now)
  *(uint64*)CLINT_MTIMECMP(cpuid) = *(uint64*)CLINT_MTIMECMP(cpuid) + TIMER_INTERVAL;
    80003d3e:	020046b7          	lui	a3,0x2004
    80003d42:	629c                	ld	a5,0(a3)
    80003d44:	000f4737          	lui	a4,0xf4
    80003d48:	24070713          	addi	a4,a4,576 # f4240 <elf_fpread-0x7ff0bdc0>
    80003d4c:	97ba                	add	a5,a5,a4
    80003d4e:	e29c                	sd	a5,0(a3)

  // setup a soft interrupt in sip (S-mode Interrupt Pending) to be handled in S-mode
  write_csr(sip, SIP_SSIP);
    80003d50:	14415073          	csrwi	sip,2
}
    80003d54:	8082                	ret

0000000080003d56 <handle_instruction_access_fault>:
static void handle_instruction_access_fault() { panic("Instruction access fault!"); }
    80003d56:	1141                	addi	sp,sp,-16
    80003d58:	e406                	sd	ra,8(sp)
    80003d5a:	00005517          	auipc	a0,0x5
    80003d5e:	e8e50513          	addi	a0,a0,-370 # 80008be8 <rfs_i_ops+0x1b70>
    80003d62:	735000ef          	jal	80004c96 <do_panic>

0000000080003d66 <handle_load_access_fault>:
static void handle_load_access_fault() { panic("Load access fault!"); }
    80003d66:	1141                	addi	sp,sp,-16
    80003d68:	e406                	sd	ra,8(sp)
    80003d6a:	00005517          	auipc	a0,0x5
    80003d6e:	e9e50513          	addi	a0,a0,-354 # 80008c08 <rfs_i_ops+0x1b90>
    80003d72:	725000ef          	jal	80004c96 <do_panic>

0000000080003d76 <handle_store_access_fault>:
static void handle_store_access_fault() { panic("Store/AMO access fault!"); }
    80003d76:	1141                	addi	sp,sp,-16
    80003d78:	e406                	sd	ra,8(sp)
    80003d7a:	00005517          	auipc	a0,0x5
    80003d7e:	ea650513          	addi	a0,a0,-346 # 80008c20 <rfs_i_ops+0x1ba8>
    80003d82:	715000ef          	jal	80004c96 <do_panic>

0000000080003d86 <handle_illegal_instruction>:
static void handle_illegal_instruction() { panic("Illegal instruction!"); }
    80003d86:	1141                	addi	sp,sp,-16
    80003d88:	e406                	sd	ra,8(sp)
    80003d8a:	00005517          	auipc	a0,0x5
    80003d8e:	eb650513          	addi	a0,a0,-330 # 80008c40 <rfs_i_ops+0x1bc8>
    80003d92:	705000ef          	jal	80004c96 <do_panic>

0000000080003d96 <handle_misaligned_load>:
static void handle_misaligned_load() { panic("Misaligned Load!"); }
    80003d96:	1141                	addi	sp,sp,-16
    80003d98:	e406                	sd	ra,8(sp)
    80003d9a:	00005517          	auipc	a0,0x5
    80003d9e:	ebe50513          	addi	a0,a0,-322 # 80008c58 <rfs_i_ops+0x1be0>
    80003da2:	6f5000ef          	jal	80004c96 <do_panic>

0000000080003da6 <handle_misaligned_store>:
static void handle_misaligned_store() { panic("Misaligned AMO!"); }
    80003da6:	1141                	addi	sp,sp,-16
    80003da8:	e406                	sd	ra,8(sp)
    80003daa:	00005517          	auipc	a0,0x5
    80003dae:	ec650513          	addi	a0,a0,-314 # 80008c70 <rfs_i_ops+0x1bf8>
    80003db2:	6e5000ef          	jal	80004c96 <do_panic>

0000000080003db6 <handle_mtrap>:

//
// handle_mtrap calls a handling function according to the type of a machine mode interrupt (trap).
//
void handle_mtrap() {
    80003db6:	1141                	addi	sp,sp,-16
    80003db8:	e406                	sd	ra,8(sp)
  uint64 mcause = read_csr(mcause);
    80003dba:	342025f3          	csrr	a1,mcause
  switch (mcause) {
    80003dbe:	479d                	li	a5,7
    80003dc0:	00b7fc63          	bgeu	a5,a1,80003dd8 <handle_mtrap+0x22>
    80003dc4:	57fd                	li	a5,-1
    80003dc6:	17fe                	slli	a5,a5,0x3f
    80003dc8:	079d                	addi	a5,a5,7
    80003dca:	04f59063          	bne	a1,a5,80003e0a <handle_mtrap+0x54>
    case CAUSE_MTIMER:
      handle_timer();
    80003dce:	f71ff0ef          	jal	80003d3e <handle_timer>
      sprint("machine trap(): unexpected mscause %p\n", mcause);
      sprint("            mepc=%p mtval=%p\n", read_csr(mepc), read_csr(mtval));
      panic( "unexpected exception happened in M-mode.\n" );
      break;
  }
}
    80003dd2:	60a2                	ld	ra,8(sp)
    80003dd4:	0141                	addi	sp,sp,16
    80003dd6:	8082                	ret
  switch (mcause) {
    80003dd8:	c98d                	beqz	a1,80003e0a <handle_mtrap+0x54>
    80003dda:	02b7e863          	bltu	a5,a1,80003e0a <handle_mtrap+0x54>
    80003dde:	00259793          	slli	a5,a1,0x2
    80003de2:	00003717          	auipc	a4,0x3
    80003de6:	3a270713          	addi	a4,a4,930 # 80007184 <rfs_i_ops+0x10c>
    80003dea:	97ba                	add	a5,a5,a4
    80003dec:	439c                	lw	a5,0(a5)
    80003dee:	97ba                	add	a5,a5,a4
    80003df0:	8782                	jr	a5
      handle_instruction_access_fault();
    80003df2:	f65ff0ef          	jal	80003d56 <handle_instruction_access_fault>
      handle_load_access_fault();
    80003df6:	f71ff0ef          	jal	80003d66 <handle_load_access_fault>
      handle_store_access_fault();
    80003dfa:	f7dff0ef          	jal	80003d76 <handle_store_access_fault>
      handle_illegal_instruction();
    80003dfe:	f89ff0ef          	jal	80003d86 <handle_illegal_instruction>
      handle_misaligned_load();
    80003e02:	f95ff0ef          	jal	80003d96 <handle_misaligned_load>
      handle_misaligned_store();
    80003e06:	fa1ff0ef          	jal	80003da6 <handle_misaligned_store>
      sprint("machine trap(): unexpected mscause %p\n", mcause);
    80003e0a:	00005517          	auipc	a0,0x5
    80003e0e:	e7e50513          	addi	a0,a0,-386 # 80008c88 <rfs_i_ops+0x1c10>
    80003e12:	5e1000ef          	jal	80004bf2 <sprint>
      sprint("            mepc=%p mtval=%p\n", read_csr(mepc), read_csr(mtval));
    80003e16:	341025f3          	csrr	a1,mepc
    80003e1a:	34302673          	csrr	a2,mtval
    80003e1e:	00005517          	auipc	a0,0x5
    80003e22:	e9250513          	addi	a0,a0,-366 # 80008cb0 <rfs_i_ops+0x1c38>
    80003e26:	5cd000ef          	jal	80004bf2 <sprint>
      panic( "unexpected exception happened in M-mode.\n" );
    80003e2a:	00005517          	auipc	a0,0x5
    80003e2e:	ea650513          	addi	a0,a0,-346 # 80008cd0 <rfs_i_ops+0x1c58>
    80003e32:	665000ef          	jal	80004c96 <do_panic>

0000000080003e36 <_mentry>:
#

.globl _mentry
_mentry:
    # [mscratch] = 0; mscratch points the stack bottom of machine mode computer
    csrw mscratch, x0
    80003e36:	34001073          	csrw	mscratch,zero

    # following codes allocate a 4096-byte stack for each HART, although we use only
    # ONE HART in this lab.
    la sp, stack0		# stack0 is statically defined in kernel/machine/minit.c 
    80003e3a:	0000b117          	auipc	sp,0xb
    80003e3e:	e6610113          	addi	sp,sp,-410 # 8000eca0 <stack0>
    li a3, 4096			# 4096-byte stack
    80003e42:	6685                	lui	a3,0x1
    csrr a4, mhartid	# [mhartid] = core ID
    80003e44:	f1402773          	csrr	a4,mhartid
    addi a4, a4, 1
    80003e48:	0705                	addi	a4,a4,1
    mul a3, a3, a4
    80003e4a:	02e686b3          	mul	a3,a3,a4
    add sp, sp, a3		# re-arrange the stack points so that they don't overlap
    80003e4e:	9136                	add	sp,sp,a3

    # jump to mstart(), i.e., machine state start function in kernel/machine/minit.c
    call m_start
    80003e50:	e5fff0ef          	jal	80003cae <m_start>
	...

0000000080003e60 <mtrapvec>:
.align 4
mtrapvec:
    # mscratch -> g_itrframe (cf. kernel/machine/minit.c line 94)
    # swap a0 and mscratch, so that a0 points to interrupt frame,
    # i.e., [a0] = &g_itrframe
    csrrw a0, mscratch, a0
    80003e60:	34051573          	csrrw	a0,mscratch,a0

    # save the registers in g_itrframe
    addi t6, a0, 0
    80003e64:	8faa                	mv	t6,a0
    store_all_registers
    80003e66:	001fb023          	sd	ra,0(t6)
    80003e6a:	002fb423          	sd	sp,8(t6)
    80003e6e:	003fb823          	sd	gp,16(t6)
    80003e72:	004fbc23          	sd	tp,24(t6)
    80003e76:	025fb023          	sd	t0,32(t6)
    80003e7a:	026fb423          	sd	t1,40(t6)
    80003e7e:	027fb823          	sd	t2,48(t6)
    80003e82:	028fbc23          	sd	s0,56(t6)
    80003e86:	049fb023          	sd	s1,64(t6)
    80003e8a:	04afb423          	sd	a0,72(t6)
    80003e8e:	04bfb823          	sd	a1,80(t6)
    80003e92:	04cfbc23          	sd	a2,88(t6)
    80003e96:	06dfb023          	sd	a3,96(t6)
    80003e9a:	06efb423          	sd	a4,104(t6)
    80003e9e:	06ffb823          	sd	a5,112(t6)
    80003ea2:	070fbc23          	sd	a6,120(t6)
    80003ea6:	091fb023          	sd	a7,128(t6)
    80003eaa:	092fb423          	sd	s2,136(t6)
    80003eae:	093fb823          	sd	s3,144(t6)
    80003eb2:	094fbc23          	sd	s4,152(t6)
    80003eb6:	0b5fb023          	sd	s5,160(t6)
    80003eba:	0b6fb423          	sd	s6,168(t6)
    80003ebe:	0b7fb823          	sd	s7,176(t6)
    80003ec2:	0b8fbc23          	sd	s8,184(t6)
    80003ec6:	0d9fb023          	sd	s9,192(t6)
    80003eca:	0dafb423          	sd	s10,200(t6)
    80003ece:	0dbfb823          	sd	s11,208(t6)
    80003ed2:	0dcfbc23          	sd	t3,216(t6)
    80003ed6:	0fdfb023          	sd	t4,224(t6)
    80003eda:	0fefb423          	sd	t5,232(t6)
    80003ede:	0fffb823          	sd	t6,240(t6)
    # save the original content of a0 in g_itrframe
    csrr t0, mscratch
    80003ee2:	340022f3          	csrr	t0,mscratch
    sd t0, 72(a0)
    80003ee6:	04553423          	sd	t0,72(a0)

    # switch stack (to use stack0) for the rest of machine mode
    # trap handling.
    la sp, stack0
    80003eea:	0000b117          	auipc	sp,0xb
    80003eee:	db610113          	addi	sp,sp,-586 # 8000eca0 <stack0>
    li a3, 4096
    80003ef2:	6685                	lui	a3,0x1
    csrr a4, mhartid
    80003ef4:	f1402773          	csrr	a4,mhartid
    addi a4, a4, 1
    80003ef8:	0705                	addi	a4,a4,1
    mul a3, a3, a4
    80003efa:	02e686b3          	mul	a3,a3,a4
    add sp, sp, a3
    80003efe:	9136                	add	sp,sp,a3

    # pointing mscratch back to g_itrframe
    csrw mscratch, a0
    80003f00:	34051073          	csrw	mscratch,a0

    # call machine mode trap handling function
    call handle_mtrap
    80003f04:	eb3ff0ef          	jal	80003db6 <handle_mtrap>

    # restore all registers, come back to the status before entering
    # machine mode handling.
    csrr t6, mscratch
    80003f08:	34002ff3          	csrr	t6,mscratch
    restore_all_registers
    80003f0c:	000fb083          	ld	ra,0(t6)
    80003f10:	008fb103          	ld	sp,8(t6)
    80003f14:	010fb183          	ld	gp,16(t6)
    80003f18:	018fb203          	ld	tp,24(t6)
    80003f1c:	020fb283          	ld	t0,32(t6)
    80003f20:	028fb303          	ld	t1,40(t6)
    80003f24:	030fb383          	ld	t2,48(t6)
    80003f28:	038fb403          	ld	s0,56(t6)
    80003f2c:	040fb483          	ld	s1,64(t6)
    80003f30:	048fb503          	ld	a0,72(t6)
    80003f34:	050fb583          	ld	a1,80(t6)
    80003f38:	058fb603          	ld	a2,88(t6)
    80003f3c:	060fb683          	ld	a3,96(t6)
    80003f40:	068fb703          	ld	a4,104(t6)
    80003f44:	070fb783          	ld	a5,112(t6)
    80003f48:	078fb803          	ld	a6,120(t6)
    80003f4c:	080fb883          	ld	a7,128(t6)
    80003f50:	088fb903          	ld	s2,136(t6)
    80003f54:	090fb983          	ld	s3,144(t6)
    80003f58:	098fba03          	ld	s4,152(t6)
    80003f5c:	0a0fba83          	ld	s5,160(t6)
    80003f60:	0a8fbb03          	ld	s6,168(t6)
    80003f64:	0b0fbb83          	ld	s7,176(t6)
    80003f68:	0b8fbc03          	ld	s8,184(t6)
    80003f6c:	0c0fbc83          	ld	s9,192(t6)
    80003f70:	0c8fbd03          	ld	s10,200(t6)
    80003f74:	0d0fbd83          	ld	s11,208(t6)
    80003f78:	0d8fbe03          	ld	t3,216(t6)
    80003f7c:	0e0fbe83          	ld	t4,224(t6)
    80003f80:	0e8fbf03          	ld	t5,232(t6)
    80003f84:	0f0fbf83          	ld	t6,240(t6)

    mret
    80003f88:	30200073          	mret
    80003f8c:	0000                	unimp
	...

0000000080003f90 <default_equal>:
#include "util/hash_table.h"
#include "util/types.h"
#include "kernel/pmm.h"

static int default_equal(void *key1, void *key2) { return key1 == key2; }
    80003f90:	8d0d                	sub	a0,a0,a1
    80003f92:	00153513          	seqz	a0,a0
    80003f96:	8082                	ret

0000000080003f98 <defalut_get>:
  node->next = head->next;
  head->next = node;
  return 0;
}

static void *defalut_get(struct hash_table *hash_table, void *key) {
    80003f98:	1101                	addi	sp,sp,-32
    80003f9a:	ec06                	sd	ra,24(sp)
    80003f9c:	e822                	sd	s0,16(sp)
    80003f9e:	e426                	sd	s1,8(sp)
    80003fa0:	e04a                	sd	s2,0(sp)
    80003fa2:	84aa                	mv	s1,a0
    80003fa4:	892e                	mv	s2,a1
  size_t index = hash_table->virtual_hash_func(key);
    80003fa6:	6785                	lui	a5,0x1
    80003fa8:	97aa                	add	a5,a5,a0
    80003faa:	c087b783          	ld	a5,-1016(a5) # c08 <elf_fpread-0x7ffff3f8>
    80003fae:	852e                	mv	a0,a1
    80003fb0:	9782                	jalr	a5
  struct hash_node *head = hash_table->head + index;
    80003fb2:	00151793          	slli	a5,a0,0x1
    80003fb6:	97aa                	add	a5,a5,a0
    80003fb8:	078e                	slli	a5,a5,0x3
    80003fba:	97a6                	add	a5,a5,s1
  struct hash_node *node = head->next;
    80003fbc:	6380                	ld	s0,0(a5)
  while (node) {
    80003fbe:	a011                	j	80003fc2 <defalut_get+0x2a>
    if (hash_table->virtual_hash_equal(node->key, key)) return node->value;
    node = node->next;
    80003fc0:	6000                	ld	s0,0(s0)
  while (node) {
    80003fc2:	c811                	beqz	s0,80003fd6 <defalut_get+0x3e>
    if (hash_table->virtual_hash_equal(node->key, key)) return node->value;
    80003fc4:	6785                	lui	a5,0x1
    80003fc6:	97a6                	add	a5,a5,s1
    80003fc8:	c007b783          	ld	a5,-1024(a5) # c00 <elf_fpread-0x7ffff400>
    80003fcc:	85ca                	mv	a1,s2
    80003fce:	6408                	ld	a0,8(s0)
    80003fd0:	9782                	jalr	a5
    80003fd2:	d57d                	beqz	a0,80003fc0 <defalut_get+0x28>
    80003fd4:	6800                	ld	s0,16(s0)
  }
  return NULL;
}
    80003fd6:	8522                	mv	a0,s0
    80003fd8:	60e2                	ld	ra,24(sp)
    80003fda:	6442                	ld	s0,16(sp)
    80003fdc:	64a2                	ld	s1,8(sp)
    80003fde:	6902                	ld	s2,0(sp)
    80003fe0:	6105                	addi	sp,sp,32
    80003fe2:	8082                	ret

0000000080003fe4 <default_erase>:

static int default_erase(struct hash_table *hash_table, void *key) {
    80003fe4:	1101                	addi	sp,sp,-32
    80003fe6:	ec06                	sd	ra,24(sp)
    80003fe8:	e822                	sd	s0,16(sp)
    80003fea:	e426                	sd	s1,8(sp)
    80003fec:	e04a                	sd	s2,0(sp)
    80003fee:	84aa                	mv	s1,a0
    80003ff0:	892e                	mv	s2,a1
  size_t index = hash_table->virtual_hash_func(key);
    80003ff2:	6785                	lui	a5,0x1
    80003ff4:	97aa                	add	a5,a5,a0
    80003ff6:	c087b783          	ld	a5,-1016(a5) # c08 <elf_fpread-0x7ffff3f8>
    80003ffa:	852e                	mv	a0,a1
    80003ffc:	9782                	jalr	a5
  struct hash_node *head = hash_table->head + index;
    80003ffe:	00151413          	slli	s0,a0,0x1
    80004002:	942a                	add	s0,s0,a0
    80004004:	040e                	slli	s0,s0,0x3
    80004006:	9426                	add	s0,s0,s1
  while (head->next && !hash_table->virtual_hash_equal(head->next->key, key))
    80004008:	a011                	j	8000400c <default_erase+0x28>
    head = head->next;
    8000400a:	6000                	ld	s0,0(s0)
  while (head->next && !hash_table->virtual_hash_equal(head->next->key, key))
    8000400c:	6018                	ld	a4,0(s0)
    8000400e:	cb09                	beqz	a4,80004020 <default_erase+0x3c>
    80004010:	6785                	lui	a5,0x1
    80004012:	97a6                	add	a5,a5,s1
    80004014:	c007b783          	ld	a5,-1024(a5) # c00 <elf_fpread-0x7ffff400>
    80004018:	85ca                	mv	a1,s2
    8000401a:	6708                	ld	a0,8(a4)
    8000401c:	9782                	jalr	a5
    8000401e:	d575                	beqz	a0,8000400a <default_erase+0x26>
  if (head->next) {
    80004020:	6008                	ld	a0,0(s0)
    80004022:	cd01                	beqz	a0,8000403a <default_erase+0x56>
    struct hash_node *node = head->next;
    head->next = node->next;
    80004024:	611c                	ld	a5,0(a0)
    80004026:	e01c                	sd	a5,0(s0)
    free_page(node);
    80004028:	f04fc0ef          	jal	8000072c <free_page>
    return 0;
    8000402c:	4501                	li	a0,0
  } else
    return -1;
}
    8000402e:	60e2                	ld	ra,24(sp)
    80004030:	6442                	ld	s0,16(sp)
    80004032:	64a2                	ld	s1,8(sp)
    80004034:	6902                	ld	s2,0(sp)
    80004036:	6105                	addi	sp,sp,32
    80004038:	8082                	ret
    return -1;
    8000403a:	557d                	li	a0,-1
    8000403c:	bfcd                	j	8000402e <default_erase+0x4a>

000000008000403e <default_put>:
static int default_put(struct hash_table *hash_table, void *key, void *value) {
    8000403e:	7179                	addi	sp,sp,-48
    80004040:	f406                	sd	ra,40(sp)
    80004042:	f022                	sd	s0,32(sp)
    80004044:	ec26                	sd	s1,24(sp)
    80004046:	e84a                	sd	s2,16(sp)
    80004048:	e44e                	sd	s3,8(sp)
    8000404a:	842a                	mv	s0,a0
    8000404c:	892e                	mv	s2,a1
    8000404e:	89b2                	mv	s3,a2
  struct hash_node *node = (struct hash_node *)alloc_page();
    80004050:	f8cfc0ef          	jal	800007dc <alloc_page>
    80004054:	84aa                	mv	s1,a0
  if (hash_table->virtual_hash_get(hash_table, key) != NULL) return -1;
    80004056:	6785                	lui	a5,0x1
    80004058:	97a2                	add	a5,a5,s0
    8000405a:	c187b783          	ld	a5,-1000(a5) # c18 <elf_fpread-0x7ffff3e8>
    8000405e:	85ca                	mv	a1,s2
    80004060:	8522                	mv	a0,s0
    80004062:	9782                	jalr	a5
    80004064:	e91d                	bnez	a0,8000409a <default_put+0x5c>
  node->key = key;
    80004066:	0124b423          	sd	s2,8(s1)
  node->value = value;
    8000406a:	0134b823          	sd	s3,16(s1)
  size_t index = hash_table->virtual_hash_func(key);
    8000406e:	6785                	lui	a5,0x1
    80004070:	97a2                	add	a5,a5,s0
    80004072:	c087b783          	ld	a5,-1016(a5) # c08 <elf_fpread-0x7ffff3f8>
    80004076:	854a                	mv	a0,s2
    80004078:	9782                	jalr	a5
  struct hash_node *head = hash_table->head + index;
    8000407a:	00151793          	slli	a5,a0,0x1
    8000407e:	953e                	add	a0,a0,a5
    80004080:	050e                	slli	a0,a0,0x3
    80004082:	942a                	add	s0,s0,a0
  node->next = head->next;
    80004084:	601c                	ld	a5,0(s0)
    80004086:	e09c                	sd	a5,0(s1)
  head->next = node;
    80004088:	e004                	sd	s1,0(s0)
  return 0;
    8000408a:	4501                	li	a0,0
}
    8000408c:	70a2                	ld	ra,40(sp)
    8000408e:	7402                	ld	s0,32(sp)
    80004090:	64e2                	ld	s1,24(sp)
    80004092:	6942                	ld	s2,16(sp)
    80004094:	69a2                	ld	s3,8(sp)
    80004096:	6145                	addi	sp,sp,48
    80004098:	8082                	ret
  if (hash_table->virtual_hash_get(hash_table, key) != NULL) return -1;
    8000409a:	557d                	li	a0,-1
    8000409c:	bfc5                	j	8000408c <default_put+0x4e>

000000008000409e <hash_table_init>:
                   int (*equal)(void *key1, void *key2),
                   size_t (*func)(void *key),
                   int (*put)(struct hash_table *hash_table, void *key, void *value),
                   void *(*get)(struct hash_table *hash_table, void *key),
                   int (*erase)(struct hash_table *hash_table, void *key)) {
  for (int i = 0; i < HASH_TABLE_SIZE; i++) list->head[i].next = NULL;
    8000409e:	4881                	li	a7,0
    800040a0:	a809                	j	800040b2 <hash_table_init+0x14>
    800040a2:	00189813          	slli	a6,a7,0x1
    800040a6:	9846                	add	a6,a6,a7
    800040a8:	080e                	slli	a6,a6,0x3
    800040aa:	982a                	add	a6,a6,a0
    800040ac:	00083023          	sd	zero,0(a6)
    800040b0:	2885                	addiw	a7,a7,1
    800040b2:	07f00813          	li	a6,127
    800040b6:	ff1856e3          	bge	a6,a7,800040a2 <hash_table_init+0x4>
  if (func == NULL) return -1;
    800040ba:	ce39                	beqz	a2,80004118 <hash_table_init+0x7a>
  list->virtual_hash_func = func;
    800040bc:	6805                	lui	a6,0x1
    800040be:	982a                	add	a6,a6,a0
    800040c0:	c0c83423          	sd	a2,-1016(a6) # c08 <elf_fpread-0x7ffff3f8>
  list->virtual_hash_equal = equal ? equal : default_equal;
    800040c4:	c595                	beqz	a1,800040f0 <hash_table_init+0x52>
    800040c6:	6605                	lui	a2,0x1
    800040c8:	962a                	add	a2,a2,a0
    800040ca:	c0b63023          	sd	a1,-1024(a2) # c00 <elf_fpread-0x7ffff400>
  list->virtual_hash_put = put ? put : default_put;
    800040ce:	c695                	beqz	a3,800040fa <hash_table_init+0x5c>
    800040d0:	6605                	lui	a2,0x1
    800040d2:	962a                	add	a2,a2,a0
    800040d4:	c0d63823          	sd	a3,-1008(a2) # c10 <elf_fpread-0x7ffff3f0>
  list->virtual_hash_get = get ? get : defalut_get;
    800040d8:	c715                	beqz	a4,80004104 <hash_table_init+0x66>
    800040da:	6685                	lui	a3,0x1
    800040dc:	96aa                	add	a3,a3,a0
    800040de:	c0e6bc23          	sd	a4,-1000(a3) # c18 <elf_fpread-0x7ffff3e8>
  list->virtual_hash_erase = erase ? erase : default_erase;
    800040e2:	c795                	beqz	a5,8000410e <hash_table_init+0x70>
    800040e4:	6705                	lui	a4,0x1
    800040e6:	953a                	add	a0,a0,a4
    800040e8:	c2f53023          	sd	a5,-992(a0)
  return 0;
    800040ec:	4501                	li	a0,0
    800040ee:	8082                	ret
  list->virtual_hash_equal = equal ? equal : default_equal;
    800040f0:	00000597          	auipc	a1,0x0
    800040f4:	ea058593          	addi	a1,a1,-352 # 80003f90 <default_equal>
    800040f8:	b7f9                	j	800040c6 <hash_table_init+0x28>
  list->virtual_hash_put = put ? put : default_put;
    800040fa:	00000697          	auipc	a3,0x0
    800040fe:	f4468693          	addi	a3,a3,-188 # 8000403e <default_put>
    80004102:	b7f9                	j	800040d0 <hash_table_init+0x32>
  list->virtual_hash_get = get ? get : defalut_get;
    80004104:	00000717          	auipc	a4,0x0
    80004108:	e9470713          	addi	a4,a4,-364 # 80003f98 <defalut_get>
    8000410c:	b7f9                	j	800040da <hash_table_init+0x3c>
  list->virtual_hash_erase = erase ? erase : default_erase;
    8000410e:	00000797          	auipc	a5,0x0
    80004112:	ed678793          	addi	a5,a5,-298 # 80003fe4 <default_erase>
    80004116:	b7f9                	j	800040e4 <hash_table_init+0x46>
  if (func == NULL) return -1;
    80004118:	557d                	li	a0,-1
    8000411a:	8082                	ret

000000008000411c <memcpy>:

void* memcpy(void* dest, const void* src, size_t len) {
  const char* s = src;
  char* d = dest;

  if ((((uintptr_t)dest | (uintptr_t)src) & (sizeof(uintptr_t) - 1)) == 0) {
    8000411c:	00b567b3          	or	a5,a0,a1
    80004120:	8b9d                	andi	a5,a5,7
    80004122:	c399                	beqz	a5,80004128 <memcpy+0xc>
  char* d = dest;
    80004124:	87aa                	mv	a5,a0
    80004126:	a015                	j	8000414a <memcpy+0x2e>
    80004128:	87aa                	mv	a5,a0
    while ((void*)d < (dest + len - (sizeof(uintptr_t) - 1))) {
    8000412a:	ff960713          	addi	a4,a2,-7
    8000412e:	972a                	add	a4,a4,a0
    80004130:	00e7fd63          	bgeu	a5,a4,8000414a <memcpy+0x2e>
      *(uintptr_t*)d = *(const uintptr_t*)s;
    80004134:	6198                	ld	a4,0(a1)
    80004136:	e398                	sd	a4,0(a5)
      d += sizeof(uintptr_t);
    80004138:	07a1                	addi	a5,a5,8
      s += sizeof(uintptr_t);
    8000413a:	05a1                	addi	a1,a1,8
    8000413c:	b7fd                	j	8000412a <memcpy+0xe>
    }
  }

  while (d < (char*)(dest + len)) *d++ = *s++;
    8000413e:	0005c703          	lbu	a4,0(a1)
    80004142:	00e78023          	sb	a4,0(a5)
    80004146:	0785                	addi	a5,a5,1
    80004148:	0585                	addi	a1,a1,1
    8000414a:	00c50733          	add	a4,a0,a2
    8000414e:	fee7e8e3          	bltu	a5,a4,8000413e <memcpy+0x22>

  return dest;
}
    80004152:	8082                	ret

0000000080004154 <memset>:

void* memset(void* dest, int byte, size_t len) {
  if ((((uintptr_t)dest | len) & (sizeof(uintptr_t) - 1)) == 0) {
    80004154:	00c567b3          	or	a5,a0,a2
    80004158:	8b9d                	andi	a5,a5,7
    8000415a:	ef8d                	bnez	a5,80004194 <memset+0x40>
    uintptr_t word = byte & 0xFF;
    8000415c:	0ff5f593          	zext.b	a1,a1
    word |= word << 8;
    80004160:	00859713          	slli	a4,a1,0x8
    80004164:	8f4d                	or	a4,a4,a1
    word |= word << 16;
    80004166:	01071793          	slli	a5,a4,0x10
    8000416a:	8fd9                	or	a5,a5,a4
    word |= word << 16 << 16;
    8000416c:	02079693          	slli	a3,a5,0x20
    80004170:	8edd                	or	a3,a3,a5

    uintptr_t* d = dest;
    80004172:	87aa                	mv	a5,a0
    while (d < (uintptr_t*)(dest + len)) *d++ = word;
    80004174:	a019                	j	8000417a <memset+0x26>
    80004176:	e394                	sd	a3,0(a5)
    80004178:	07a1                	addi	a5,a5,8
    8000417a:	00c50733          	add	a4,a0,a2
    8000417e:	fee7ece3          	bltu	a5,a4,80004176 <memset+0x22>
    80004182:	8082                	ret
  } else {
    char* d = dest;
    while (d < (char*)(dest + len)) *d++ = byte;
    80004184:	00b78023          	sb	a1,0(a5)
    80004188:	0785                	addi	a5,a5,1
    8000418a:	00c50733          	add	a4,a0,a2
    8000418e:	fee7ebe3          	bltu	a5,a4,80004184 <memset+0x30>
  }
  return dest;
}
    80004192:	8082                	ret
    char* d = dest;
    80004194:	87aa                	mv	a5,a0
    80004196:	bfd5                	j	8000418a <memset+0x36>

0000000080004198 <strlen>:

size_t strlen(const char* s) {
  const char* p = s;
    80004198:	87aa                	mv	a5,a0
  while (*p) p++;
    8000419a:	a011                	j	8000419e <strlen+0x6>
    8000419c:	0785                	addi	a5,a5,1
    8000419e:	0007c703          	lbu	a4,0(a5)
    800041a2:	ff6d                	bnez	a4,8000419c <strlen+0x4>
  return p - s;
}
    800041a4:	40a78533          	sub	a0,a5,a0
    800041a8:	8082                	ret

00000000800041aa <strcmp>:

int strcmp(const char* s1, const char* s2) {
  unsigned char c1, c2;

  do {
    c1 = *s1++;
    800041aa:	00054703          	lbu	a4,0(a0)
    800041ae:	0505                	addi	a0,a0,1
    c2 = *s2++;
    800041b0:	0005c783          	lbu	a5,0(a1)
    800041b4:	0585                	addi	a1,a1,1
  } while (c1 != 0 && c1 == c2);
    800041b6:	c319                	beqz	a4,800041bc <strcmp+0x12>
    800041b8:	fef709e3          	beq	a4,a5,800041aa <strcmp>

  return c1 - c2;
}
    800041bc:	40f7053b          	subw	a0,a4,a5
    800041c0:	8082                	ret

00000000800041c2 <strcpy>:

char* strcpy(char* dest, const char* src) {
  char* d = dest;
    800041c2:	86aa                	mv	a3,a0
  while ((*d++ = *src++))
    800041c4:	0005c703          	lbu	a4,0(a1)
    800041c8:	0585                	addi	a1,a1,1
    800041ca:	00e68023          	sb	a4,0(a3)
    800041ce:	0685                	addi	a3,a3,1
    800041d0:	fb75                	bnez	a4,800041c4 <strcpy+0x2>
    ;
  return dest;
}
    800041d2:	8082                	ret

00000000800041d4 <strchr>:

char *strchr(const char *p, int ch)
{
	char c;
	c = ch;
    800041d4:	0ff5f593          	zext.b	a1,a1
    800041d8:	a011                	j	800041dc <strchr+0x8>
	for (;; ++p) {
    800041da:	0505                	addi	a0,a0,1
		if (*p == c)
    800041dc:	00054783          	lbu	a5,0(a0)
    800041e0:	00b78463          	beq	a5,a1,800041e8 <strchr+0x14>
			return ((char *)p);
		if (*p == '\0')
    800041e4:	fbfd                	bnez	a5,800041da <strchr+0x6>
			return (NULL);
    800041e6:	4501                	li	a0,0
	}
}
    800041e8:	8082                	ret

00000000800041ea <strtok>:

char* strtok(char* str, const char* delim) {
    800041ea:	7179                	addi	sp,sp,-48
    800041ec:	f406                	sd	ra,40(sp)
    800041ee:	f022                	sd	s0,32(sp)
    800041f0:	e44e                	sd	s3,8(sp)
    800041f2:	89ae                	mv	s3,a1
  static char* current;
  if (str != NULL) current = str;
    800041f4:	c509                	beqz	a0,800041fe <strtok+0x14>
    800041f6:	0000c797          	auipc	a5,0xc
    800041fa:	f4a7bd23          	sd	a0,-166(a5) # 80010150 <current.0>
  if (current == NULL) return NULL;
    800041fe:	0000c417          	auipc	s0,0xc
    80004202:	f5243403          	ld	s0,-174(s0) # 80010150 <current.0>
    80004206:	cc21                	beqz	s0,8000425e <strtok+0x74>
    80004208:	ec26                	sd	s1,24(sp)
    8000420a:	a011                	j	8000420e <strtok+0x24>

  char* start = current;
  while (*start != '\0' && strchr(delim, *start) != NULL) start++;
    8000420c:	0405                	addi	s0,s0,1
    8000420e:	00044483          	lbu	s1,0(s0)
    80004212:	c491                	beqz	s1,8000421e <strtok+0x34>
    80004214:	85a6                	mv	a1,s1
    80004216:	854e                	mv	a0,s3
    80004218:	fbdff0ef          	jal	800041d4 <strchr>
    8000421c:	f965                	bnez	a0,8000420c <strtok+0x22>

  if (*start == '\0') {
    8000421e:	cc91                	beqz	s1,8000423a <strtok+0x50>
    80004220:	e84a                	sd	s2,16(sp)
    current = NULL;
    return current;
  }

  char* end = start;
    80004222:	84a2                	mv	s1,s0
  while (*end != '\0' && strchr(delim, *end) == NULL) end++;
    80004224:	0004c903          	lbu	s2,0(s1)
    80004228:	02090063          	beqz	s2,80004248 <strtok+0x5e>
    8000422c:	85ca                	mv	a1,s2
    8000422e:	854e                	mv	a0,s3
    80004230:	fa5ff0ef          	jal	800041d4 <strchr>
    80004234:	e911                	bnez	a0,80004248 <strtok+0x5e>
    80004236:	0485                	addi	s1,s1,1
    80004238:	b7f5                	j	80004224 <strtok+0x3a>
    current = NULL;
    8000423a:	0000c797          	auipc	a5,0xc
    8000423e:	f007bb23          	sd	zero,-234(a5) # 80010150 <current.0>
    return current;
    80004242:	4401                	li	s0,0
    80004244:	64e2                	ld	s1,24(sp)
    80004246:	a821                	j	8000425e <strtok+0x74>

  if (*end != '\0') {
    80004248:	02090163          	beqz	s2,8000426a <strtok+0x80>
    *end = '\0';
    8000424c:	00048023          	sb	zero,0(s1)
    current = end + 1;
    80004250:	0485                	addi	s1,s1,1
    80004252:	0000c797          	auipc	a5,0xc
    80004256:	ee97bf23          	sd	s1,-258(a5) # 80010150 <current.0>
    8000425a:	64e2                	ld	s1,24(sp)
    8000425c:	6942                	ld	s2,16(sp)
  } else
    current = NULL;
  return start;
}
    8000425e:	8522                	mv	a0,s0
    80004260:	70a2                	ld	ra,40(sp)
    80004262:	7402                	ld	s0,32(sp)
    80004264:	69a2                	ld	s3,8(sp)
    80004266:	6145                	addi	sp,sp,48
    80004268:	8082                	ret
    current = NULL;
    8000426a:	0000c797          	auipc	a5,0xc
    8000426e:	ee07b323          	sd	zero,-282(a5) # 80010150 <current.0>
    80004272:	64e2                	ld	s1,24(sp)
    80004274:	6942                	ld	s2,16(sp)
    80004276:	b7e5                	j	8000425e <strtok+0x74>

0000000080004278 <strcat>:

char *strcat(char *dst, const char *src) {
    80004278:	1101                	addi	sp,sp,-32
    8000427a:	ec06                	sd	ra,24(sp)
    8000427c:	e822                	sd	s0,16(sp)
    8000427e:	e426                	sd	s1,8(sp)
    80004280:	842a                	mv	s0,a0
    80004282:	84ae                	mv	s1,a1
  strcpy(dst + strlen(dst), src);
    80004284:	f15ff0ef          	jal	80004198 <strlen>
    80004288:	85a6                	mv	a1,s1
    8000428a:	9522                	add	a0,a0,s0
    8000428c:	f37ff0ef          	jal	800041c2 <strcpy>
  return dst;
}
    80004290:	8522                	mv	a0,s0
    80004292:	60e2                	ld	ra,24(sp)
    80004294:	6442                	ld	s0,16(sp)
    80004296:	64a2                	ld	s1,8(sp)
    80004298:	6105                	addi	sp,sp,32
    8000429a:	8082                	ret

000000008000429c <atol>:

long atol(const char* str) {
    8000429c:	87aa                	mv	a5,a0
  long res = 0;
  int sign = 0;

  while (*str == ' ') str++;
    8000429e:	a011                	j	800042a2 <atol+0x6>
    800042a0:	0785                	addi	a5,a5,1
    800042a2:	0007c703          	lbu	a4,0(a5)
    800042a6:	02000693          	li	a3,32
    800042aa:	fed70be3          	beq	a4,a3,800042a0 <atol+0x4>

  if (*str == '-' || *str == '+') {
    800042ae:	02d00693          	li	a3,45
    800042b2:	00d70863          	beq	a4,a3,800042c2 <atol+0x26>
    800042b6:	02b00693          	li	a3,43
    800042ba:	00d70463          	beq	a4,a3,800042c2 <atol+0x26>
  int sign = 0;
    800042be:	4601                	li	a2,0
    800042c0:	a031                	j	800042cc <atol+0x30>
    sign = *str == '-';
    800042c2:	fd370713          	addi	a4,a4,-45
    800042c6:	00173613          	seqz	a2,a4
    str++;
    800042ca:	0785                	addi	a5,a5,1
  int sign = 0;
    800042cc:	4501                	li	a0,0
    800042ce:	a811                	j	800042e2 <atol+0x46>
  }

  while (*str) {
    res *= 10;
    800042d0:	00251713          	slli	a4,a0,0x2
    800042d4:	972a                	add	a4,a4,a0
    800042d6:	0706                	slli	a4,a4,0x1
    res += *str++ - '0';
    800042d8:	0785                	addi	a5,a5,1
    800042da:	fd06869b          	addiw	a3,a3,-48
    800042de:	00e68533          	add	a0,a3,a4
  while (*str) {
    800042e2:	0007c683          	lbu	a3,0(a5)
    800042e6:	f6ed                	bnez	a3,800042d0 <atol+0x34>
  }

  return sign ? -res : res;
    800042e8:	c219                	beqz	a2,800042ee <atol+0x52>
    800042ea:	40a00533          	neg	a0,a0
}
    800042ee:	8082                	ret

00000000800042f0 <memmove>:
  const char* s;
  char* d;

  s = src;
  d = dst;
  if (s < d && s + n > d) {
    800042f0:	02a5ff63          	bgeu	a1,a0,8000432e <memmove+0x3e>
    800042f4:	00c587b3          	add	a5,a1,a2
    800042f8:	02f57d63          	bgeu	a0,a5,80004332 <memmove+0x42>
    s += n;
    d += n;
    800042fc:	00c50733          	add	a4,a0,a2
    while (n-- > 0) *--d = *--s;
    80004300:	a801                	j	80004310 <memmove+0x20>
    80004302:	17fd                	addi	a5,a5,-1
    80004304:	177d                	addi	a4,a4,-1
    80004306:	0007c603          	lbu	a2,0(a5)
    8000430a:	00c70023          	sb	a2,0(a4)
    8000430e:	8636                	mv	a2,a3
    80004310:	fff60693          	addi	a3,a2,-1
    80004314:	f67d                	bnez	a2,80004302 <memmove+0x12>
    80004316:	8082                	ret
  } else
    while (n-- > 0) *d++ = *s++;
    80004318:	0005c683          	lbu	a3,0(a1)
    8000431c:	00d78023          	sb	a3,0(a5)
    80004320:	0785                	addi	a5,a5,1
    80004322:	0585                	addi	a1,a1,1
    80004324:	863a                	mv	a2,a4
    80004326:	fff60713          	addi	a4,a2,-1
    8000432a:	f67d                	bnez	a2,80004318 <memmove+0x28>

  return dst;
}
    8000432c:	8082                	ret
    8000432e:	87aa                	mv	a5,a0
    80004330:	bfdd                	j	80004326 <memmove+0x36>
    80004332:	87aa                	mv	a5,a0
    80004334:	bfcd                	j	80004326 <memmove+0x36>

0000000080004336 <safestrcpy>:
// Like strncpy but guaranteed to NUL-terminate.
char* safestrcpy(char* s, const char* t, int n) {
  char* os;

  os = s;
  if (n <= 0) return os;
    80004336:	02c05563          	blez	a2,80004360 <safestrcpy+0x2a>
    8000433a:	87aa                	mv	a5,a0
    8000433c:	a019                	j	80004342 <safestrcpy+0xc>
  while (--n > 0 && (*s++ = *t++) != 0)
    8000433e:	85c2                	mv	a1,a6
    80004340:	87b6                	mv	a5,a3
    80004342:	367d                	addiw	a2,a2,-1
    80004344:	00c05c63          	blez	a2,8000435c <safestrcpy+0x26>
    80004348:	00158813          	addi	a6,a1,1
    8000434c:	00178693          	addi	a3,a5,1
    80004350:	0005c703          	lbu	a4,0(a1)
    80004354:	00e78023          	sb	a4,0(a5)
    80004358:	f37d                	bnez	a4,8000433e <safestrcpy+0x8>
    8000435a:	87b6                	mv	a5,a3
    ;
  *s = 0;
    8000435c:	00078023          	sb	zero,0(a5)
  return os;
}
    80004360:	8082                	ret

0000000080004362 <spike_file_get_free>:
ssize_t spike_file_write(spike_file_t* f, const void* buf, size_t size) {
  return frontend_syscall(HTIFSYS_write, f->kfd, (uint64)buf, size, 0, 0, 0, 0);
}

static spike_file_t* spike_file_get_free(void) {
  for (spike_file_t* f = spike_files; f < spike_files + MAX_FILES; f++)
    80004362:	00006517          	auipc	a0,0x6
    80004366:	c9e50513          	addi	a0,a0,-866 # 8000a000 <spike_files>
    8000436a:	a011                	j	8000436e <spike_file_get_free+0xc>
    8000436c:	0521                	addi	a0,a0,8
    8000436e:	00006797          	auipc	a5,0x6
    80004372:	09278793          	addi	a5,a5,146 # 8000a400 <spike_files+0x400>
    80004376:	00f57b63          	bgeu	a0,a5,8000438c <spike_file_get_free+0x2a>
    if (atomic_read(&f->refcnt) == 0 && atomic_cas(&f->refcnt, 0, INIT_FILE_REF) == 0)
    8000437a:	415c                	lw	a5,4(a0)
    8000437c:	2781                	sext.w	a5,a5
    8000437e:	f7fd                	bnez	a5,8000436c <spike_file_get_free+0xa>
    80004380:	415c                	lw	a5,4(a0)
    80004382:	2781                	sext.w	a5,a5
    80004384:	f7e5                	bnez	a5,8000436c <spike_file_get_free+0xa>
    80004386:	478d                	li	a5,3
    80004388:	c15c                	sw	a5,4(a0)
      return f;
    8000438a:	8082                	ret
  return NULL;
    8000438c:	4501                	li	a0,0
}
    8000438e:	8082                	ret

0000000080004390 <copy_stat>:
  dest->st_dev = src->dev;
    80004390:	619c                	ld	a5,0(a1)
    80004392:	00f51023          	sh	a5,0(a0)
  dest->st_ino = src->ino;
    80004396:	659c                	ld	a5,8(a1)
    80004398:	00f51123          	sh	a5,2(a0)
  dest->st_mode = src->mode;
    8000439c:	499c                	lw	a5,16(a1)
    8000439e:	c15c                	sw	a5,4(a0)
  dest->st_nlink = src->nlink;
    800043a0:	49dc                	lw	a5,20(a1)
    800043a2:	00f51423          	sh	a5,8(a0)
  dest->st_uid = src->uid;
    800043a6:	4d9c                	lw	a5,24(a1)
    800043a8:	00f51523          	sh	a5,10(a0)
  dest->st_gid = src->gid;
    800043ac:	4ddc                	lw	a5,28(a1)
    800043ae:	00f51623          	sh	a5,12(a0)
  dest->st_rdev = src->rdev;
    800043b2:	719c                	ld	a5,32(a1)
    800043b4:	00f51723          	sh	a5,14(a0)
  dest->st_size = src->size;
    800043b8:	799c                	ld	a5,48(a1)
    800043ba:	e91c                	sd	a5,16(a0)
  dest->st_blksize = src->blksize;
    800043bc:	0385e783          	lwu	a5,56(a1)
    800043c0:	e53c                	sd	a5,72(a0)
  dest->st_blocks = src->blocks;
    800043c2:	61bc                	ld	a5,64(a1)
    800043c4:	e93c                	sd	a5,80(a0)
  dest->st_atime = src->atime;
    800043c6:	65bc                	ld	a5,72(a1)
    800043c8:	ed1c                	sd	a5,24(a0)
  dest->st_mtime = src->mtime;
    800043ca:	6dbc                	ld	a5,88(a1)
    800043cc:	f51c                	sd	a5,40(a0)
  dest->st_ctime = src->ctime;
    800043ce:	75bc                	ld	a5,104(a1)
    800043d0:	fd1c                	sd	a5,56(a0)
}
    800043d2:	8082                	ret

00000000800043d4 <spike_file_stat>:
int spike_file_stat(spike_file_t* f, struct stat* s) {
    800043d4:	7135                	addi	sp,sp,-160
    800043d6:	ed06                	sd	ra,152(sp)
    800043d8:	e922                	sd	s0,144(sp)
    800043da:	e526                	sd	s1,136(sp)
    800043dc:	84ae                	mv	s1,a1
  long ret = frontend_syscall(HTIFSYS_fstat, f->kfd, (uint64)&buf, 0, 0, 0, 0, 0);
    800043de:	4881                	li	a7,0
    800043e0:	4801                	li	a6,0
    800043e2:	4781                	li	a5,0
    800043e4:	4701                	li	a4,0
    800043e6:	4681                	li	a3,0
    800043e8:	860a                	mv	a2,sp
    800043ea:	410c                	lw	a1,0(a0)
    800043ec:	05000513          	li	a0,80
    800043f0:	6fe000ef          	jal	80004aee <frontend_syscall>
    800043f4:	842a                	mv	s0,a0
  copy_stat(s, &buf);
    800043f6:	858a                	mv	a1,sp
    800043f8:	8526                	mv	a0,s1
    800043fa:	f97ff0ef          	jal	80004390 <copy_stat>
}
    800043fe:	0004051b          	sext.w	a0,s0
    80004402:	60ea                	ld	ra,152(sp)
    80004404:	644a                	ld	s0,144(sp)
    80004406:	64aa                	ld	s1,136(sp)
    80004408:	610d                	addi	sp,sp,160
    8000440a:	8082                	ret

000000008000440c <spike_file_decref>:
  if (atomic_add(&f->refcnt, -1) == 2) {
    8000440c:	415c                	lw	a5,4(a0)
    8000440e:	0007871b          	sext.w	a4,a5
    80004412:	37fd                	addiw	a5,a5,-1
    80004414:	c15c                	sw	a5,4(a0)
    80004416:	4789                	li	a5,2
    80004418:	00f70363          	beq	a4,a5,8000441e <spike_file_decref+0x12>
    8000441c:	8082                	ret
void spike_file_decref(spike_file_t* f) {
    8000441e:	1141                	addi	sp,sp,-16
    80004420:	e406                	sd	ra,8(sp)
    int kfd = f->kfd;
    80004422:	410c                	lw	a1,0(a0)
    mb();
    80004424:	0ff0000f          	fence
    atomic_set(&f->refcnt, 0);
    80004428:	00052223          	sw	zero,4(a0)
    frontend_syscall(HTIFSYS_close, kfd, 0, 0, 0, 0, 0, 0);
    8000442c:	4881                	li	a7,0
    8000442e:	4801                	li	a6,0
    80004430:	4781                	li	a5,0
    80004432:	4701                	li	a4,0
    80004434:	4681                	li	a3,0
    80004436:	4601                	li	a2,0
    80004438:	03900513          	li	a0,57
    8000443c:	6b2000ef          	jal	80004aee <frontend_syscall>
}
    80004440:	60a2                	ld	ra,8(sp)
    80004442:	0141                	addi	sp,sp,16
    80004444:	8082                	ret

0000000080004446 <spike_file_close>:
  if (!f) return -1;
    80004446:	c539                	beqz	a0,80004494 <spike_file_close+0x4e>
int spike_file_close(spike_file_t* f) {
    80004448:	1101                	addi	sp,sp,-32
    8000444a:	ec06                	sd	ra,24(sp)
    8000444c:	e822                	sd	s0,16(sp)
    8000444e:	e426                	sd	s1,8(sp)
    80004450:	842a                	mv	s0,a0
  spike_file_t* old = atomic_cas(&spike_fds[f->kfd], f, 0);
    80004452:	4118                	lw	a4,0(a0)
    80004454:	00371693          	slli	a3,a4,0x3
    80004458:	0000c797          	auipc	a5,0xc
    8000445c:	84878793          	addi	a5,a5,-1976 # 8000fca0 <spike_fds>
    80004460:	97b6                	add	a5,a5,a3
    80004462:	6384                	ld	s1,0(a5)
    80004464:	02950063          	beq	a0,s1,80004484 <spike_file_close+0x3e>
  spike_file_decref(f);
    80004468:	8522                	mv	a0,s0
    8000446a:	fa3ff0ef          	jal	8000440c <spike_file_decref>
  if (old != f) return -1;
    8000446e:	02941563          	bne	s0,s1,80004498 <spike_file_close+0x52>
  spike_file_decref(f);
    80004472:	8522                	mv	a0,s0
    80004474:	f99ff0ef          	jal	8000440c <spike_file_decref>
  return 0;
    80004478:	4501                	li	a0,0
}
    8000447a:	60e2                	ld	ra,24(sp)
    8000447c:	6442                	ld	s0,16(sp)
    8000447e:	64a2                	ld	s1,8(sp)
    80004480:	6105                	addi	sp,sp,32
    80004482:	8082                	ret
  spike_file_t* old = atomic_cas(&spike_fds[f->kfd], f, 0);
    80004484:	0000c797          	auipc	a5,0xc
    80004488:	81c78793          	addi	a5,a5,-2020 # 8000fca0 <spike_fds>
    8000448c:	97b6                	add	a5,a5,a3
    8000448e:	0007b023          	sd	zero,0(a5)
    80004492:	bfd9                	j	80004468 <spike_file_close+0x22>
  if (!f) return -1;
    80004494:	557d                	li	a0,-1
}
    80004496:	8082                	ret
  if (old != f) return -1;
    80004498:	557d                	li	a0,-1
    8000449a:	b7c5                	j	8000447a <spike_file_close+0x34>

000000008000449c <spike_file_incref>:
  long prev = atomic_add(&f->refcnt, 1);
    8000449c:	415c                	lw	a5,4(a0)
    8000449e:	0007871b          	sext.w	a4,a5
    800044a2:	2785                	addiw	a5,a5,1
    800044a4:	c15c                	sw	a5,4(a0)
  kassert(prev > 0);
    800044a6:	c311                	beqz	a4,800044aa <spike_file_incref+0xe>
    800044a8:	8082                	ret
void spike_file_incref(spike_file_t* f) {
    800044aa:	1141                	addi	sp,sp,-16
    800044ac:	e406                	sd	ra,8(sp)
  kassert(prev > 0);
    800044ae:	00005517          	auipc	a0,0x5
    800044b2:	85250513          	addi	a0,a0,-1966 # 80008d00 <rfs_i_ops+0x1c88>
    800044b6:	001000ef          	jal	80004cb6 <kassert_fail>

00000000800044ba <spike_file_write>:
ssize_t spike_file_write(spike_file_t* f, const void* buf, size_t size) {
    800044ba:	1141                	addi	sp,sp,-16
    800044bc:	e406                	sd	ra,8(sp)
    800044be:	86b2                	mv	a3,a2
  return frontend_syscall(HTIFSYS_write, f->kfd, (uint64)buf, size, 0, 0, 0, 0);
    800044c0:	4881                	li	a7,0
    800044c2:	4801                	li	a6,0
    800044c4:	4781                	li	a5,0
    800044c6:	4701                	li	a4,0
    800044c8:	862e                	mv	a2,a1
    800044ca:	410c                	lw	a1,0(a0)
    800044cc:	04000513          	li	a0,64
    800044d0:	61e000ef          	jal	80004aee <frontend_syscall>
}
    800044d4:	60a2                	ld	ra,8(sp)
    800044d6:	0141                	addi	sp,sp,16
    800044d8:	8082                	ret

00000000800044da <spike_file_dup>:

int spike_file_dup(spike_file_t* f) {
    800044da:	1141                	addi	sp,sp,-16
    800044dc:	e406                	sd	ra,8(sp)
    800044de:	e022                	sd	s0,0(sp)
  for (int i = 0; i < MAX_FDS; i++) {
    800044e0:	4401                	li	s0,0
    800044e2:	07f00793          	li	a5,127
    800044e6:	0287cc63          	blt	a5,s0,8000451e <spike_file_dup+0x44>
    if (atomic_cas(&spike_fds[i], 0, f) == 0) {
    800044ea:	00341713          	slli	a4,s0,0x3
    800044ee:	0000b797          	auipc	a5,0xb
    800044f2:	7b278793          	addi	a5,a5,1970 # 8000fca0 <spike_fds>
    800044f6:	97ba                	add	a5,a5,a4
    800044f8:	639c                	ld	a5,0(a5)
    800044fa:	c399                	beqz	a5,80004500 <spike_file_dup+0x26>
  for (int i = 0; i < MAX_FDS; i++) {
    800044fc:	2405                	addiw	s0,s0,1
    800044fe:	b7d5                	j	800044e2 <spike_file_dup+0x8>
    if (atomic_cas(&spike_fds[i], 0, f) == 0) {
    80004500:	00341713          	slli	a4,s0,0x3
    80004504:	0000b797          	auipc	a5,0xb
    80004508:	79c78793          	addi	a5,a5,1948 # 8000fca0 <spike_fds>
    8000450c:	97ba                	add	a5,a5,a4
    8000450e:	e388                	sd	a0,0(a5)
      spike_file_incref(f);
    80004510:	f8dff0ef          	jal	8000449c <spike_file_incref>
      return i;
    }
  }
  return -1;
}
    80004514:	8522                	mv	a0,s0
    80004516:	60a2                	ld	ra,8(sp)
    80004518:	6402                	ld	s0,0(sp)
    8000451a:	0141                	addi	sp,sp,16
    8000451c:	8082                	ret
  return -1;
    8000451e:	547d                	li	s0,-1
    80004520:	bfd5                	j	80004514 <spike_file_dup+0x3a>

0000000080004522 <spike_file_init>:

void spike_file_init(void) {
    80004522:	1141                	addi	sp,sp,-16
    80004524:	e406                	sd	ra,8(sp)
    80004526:	e022                	sd	s0,0(sp)
  // create stdin, stdout, stderr and FDs 0-2
  for (int i = 0; i < 3; i++) {
    80004528:	4401                	li	s0,0
    8000452a:	a039                	j	80004538 <spike_file_init+0x16>
    spike_file_t* f = spike_file_get_free();
    8000452c:	e37ff0ef          	jal	80004362 <spike_file_get_free>
    f->kfd = i;
    80004530:	c100                	sw	s0,0(a0)
    spike_file_dup(f);
    80004532:	fa9ff0ef          	jal	800044da <spike_file_dup>
  for (int i = 0; i < 3; i++) {
    80004536:	2405                	addiw	s0,s0,1
    80004538:	4789                	li	a5,2
    8000453a:	fe87d9e3          	bge	a5,s0,8000452c <spike_file_init+0xa>
  }
}
    8000453e:	60a2                	ld	ra,8(sp)
    80004540:	6402                	ld	s0,0(sp)
    80004542:	0141                	addi	sp,sp,16
    80004544:	8082                	ret

0000000080004546 <spike_file_openat>:

spike_file_t* spike_file_openat(int dirfd, const char* fn, int flags, int mode) {
    80004546:	7179                	addi	sp,sp,-48
    80004548:	f406                	sd	ra,40(sp)
    8000454a:	f022                	sd	s0,32(sp)
    8000454c:	ec26                	sd	s1,24(sp)
    8000454e:	e84a                	sd	s2,16(sp)
    80004550:	e44e                	sd	s3,8(sp)
    80004552:	e052                	sd	s4,0(sp)
    80004554:	892a                	mv	s2,a0
    80004556:	84ae                	mv	s1,a1
    80004558:	89b2                	mv	s3,a2
    8000455a:	8a36                	mv	s4,a3
  spike_file_t* f = spike_file_get_free();
    8000455c:	e07ff0ef          	jal	80004362 <spike_file_get_free>
  if (f == NULL) return ERR_PTR(-ENOMEM);
    80004560:	c139                	beqz	a0,800045a6 <spike_file_openat+0x60>
    80004562:	842a                	mv	s0,a0

  size_t fn_size = strlen(fn) + 1;
    80004564:	8526                	mv	a0,s1
    80004566:	c33ff0ef          	jal	80004198 <strlen>
  long ret = frontend_syscall(HTIFSYS_openat, dirfd, (uint64)fn, fn_size, flags, mode, 0, 0);
    8000456a:	4881                	li	a7,0
    8000456c:	4801                	li	a6,0
    8000456e:	87d2                	mv	a5,s4
    80004570:	874e                	mv	a4,s3
    80004572:	00150693          	addi	a3,a0,1
    80004576:	8626                	mv	a2,s1
    80004578:	85ca                	mv	a1,s2
    8000457a:	03800513          	li	a0,56
    8000457e:	570000ef          	jal	80004aee <frontend_syscall>
    80004582:	84aa                	mv	s1,a0
  if (ret >= 0) {
    80004584:	00054c63          	bltz	a0,8000459c <spike_file_openat+0x56>
    f->kfd = ret;
    80004588:	c008                	sw	a0,0(s0)
    return f;
  } else {
    spike_file_decref(f);
    return ERR_PTR(ret);
  }
}
    8000458a:	8522                	mv	a0,s0
    8000458c:	70a2                	ld	ra,40(sp)
    8000458e:	7402                	ld	s0,32(sp)
    80004590:	64e2                	ld	s1,24(sp)
    80004592:	6942                	ld	s2,16(sp)
    80004594:	69a2                	ld	s3,8(sp)
    80004596:	6a02                	ld	s4,0(sp)
    80004598:	6145                	addi	sp,sp,48
    8000459a:	8082                	ret
    spike_file_decref(f);
    8000459c:	8522                	mv	a0,s0
    8000459e:	e6fff0ef          	jal	8000440c <spike_file_decref>
    return ERR_PTR(ret);
    800045a2:	8426                	mv	s0,s1
    800045a4:	b7dd                	j	8000458a <spike_file_openat+0x44>
  if (f == NULL) return ERR_PTR(-ENOMEM);
    800045a6:	5451                	li	s0,-12
    800045a8:	b7cd                	j	8000458a <spike_file_openat+0x44>

00000000800045aa <spike_file_open>:

spike_file_t* spike_file_open(const char* fn, int flags, int mode) {
    800045aa:	1141                	addi	sp,sp,-16
    800045ac:	e406                	sd	ra,8(sp)
    800045ae:	86b2                	mv	a3,a2
  return spike_file_openat(AT_FDCWD, fn, flags, mode);
    800045b0:	862e                	mv	a2,a1
    800045b2:	85aa                	mv	a1,a0
    800045b4:	f9c00513          	li	a0,-100
    800045b8:	f8fff0ef          	jal	80004546 <spike_file_openat>
}
    800045bc:	60a2                	ld	ra,8(sp)
    800045be:	0141                	addi	sp,sp,16
    800045c0:	8082                	ret

00000000800045c2 <spike_file_pread>:

ssize_t spike_file_pread(spike_file_t* f, void* buf, size_t size, off_t offset) {
    800045c2:	1141                	addi	sp,sp,-16
    800045c4:	e406                	sd	ra,8(sp)
    800045c6:	8736                	mv	a4,a3
  return frontend_syscall(HTIFSYS_pread, f->kfd, (uint64)buf, size, offset, 0, 0, 0);
    800045c8:	4881                	li	a7,0
    800045ca:	4801                	li	a6,0
    800045cc:	4781                	li	a5,0
    800045ce:	86b2                	mv	a3,a2
    800045d0:	862e                	mv	a2,a1
    800045d2:	410c                	lw	a1,0(a0)
    800045d4:	04300513          	li	a0,67
    800045d8:	516000ef          	jal	80004aee <frontend_syscall>
}
    800045dc:	60a2                	ld	ra,8(sp)
    800045de:	0141                	addi	sp,sp,16
    800045e0:	8082                	ret

00000000800045e2 <spike_file_read>:

ssize_t spike_file_read(spike_file_t* f, void* buf, size_t size) {
    800045e2:	1141                	addi	sp,sp,-16
    800045e4:	e406                	sd	ra,8(sp)
    800045e6:	86b2                	mv	a3,a2
  return frontend_syscall(HTIFSYS_read, f->kfd, (uint64)buf, size, 0, 0, 0, 0);
    800045e8:	4881                	li	a7,0
    800045ea:	4801                	li	a6,0
    800045ec:	4781                	li	a5,0
    800045ee:	4701                	li	a4,0
    800045f0:	862e                	mv	a2,a1
    800045f2:	410c                	lw	a1,0(a0)
    800045f4:	03f00513          	li	a0,63
    800045f8:	4f6000ef          	jal	80004aee <frontend_syscall>
}
    800045fc:	60a2                	ld	ra,8(sp)
    800045fe:	0141                	addi	sp,sp,16
    80004600:	8082                	ret

0000000080004602 <spike_file_lseek>:

ssize_t spike_file_lseek(spike_file_t* f, size_t ptr, int dir) {
    80004602:	1141                	addi	sp,sp,-16
    80004604:	e406                	sd	ra,8(sp)
    80004606:	86b2                	mv	a3,a2
  return frontend_syscall(HTIFSYS_lseek, f->kfd, ptr, dir, 0, 0, 0, 0);
    80004608:	4881                	li	a7,0
    8000460a:	4801                	li	a6,0
    8000460c:	4781                	li	a5,0
    8000460e:	4701                	li	a4,0
    80004610:	862e                	mv	a2,a1
    80004612:	410c                	lw	a1,0(a0)
    80004614:	03e00513          	li	a0,62
    80004618:	4d6000ef          	jal	80004aee <frontend_syscall>
}
    8000461c:	60a2                	ld	ra,8(sp)
    8000461e:	0141                	addi	sp,sp,16
    80004620:	8082                	ret

0000000080004622 <spike_file_get>:

spike_file_t* spike_file_get(int fd) {
  spike_file_t* f;
  if (fd < 0 || fd >= MAX_FDS || (f = atomic_read(&spike_fds[fd])) == NULL)
    80004622:	0005071b          	sext.w	a4,a0
    80004626:	07f00793          	li	a5,127
    8000462a:	02e7ec63          	bltu	a5,a4,80004662 <spike_file_get+0x40>
    8000462e:	050e                	slli	a0,a0,0x3
    80004630:	0000b797          	auipc	a5,0xb
    80004634:	67078793          	addi	a5,a5,1648 # 8000fca0 <spike_fds>
    80004638:	97aa                	add	a5,a5,a0
    8000463a:	6388                	ld	a0,0(a5)
    8000463c:	e501                	bnez	a0,80004644 <spike_file_get+0x22>
    8000463e:	8082                	ret
  long old_cnt;
  do {
    old_cnt = atomic_read(&f->refcnt);
    if (old_cnt == 0)
      return 0;
  } while (atomic_cas(&f->refcnt, old_cnt, old_cnt+1) != old_cnt);
    80004640:	02f70063          	beq	a4,a5,80004660 <spike_file_get+0x3e>
    old_cnt = atomic_read(&f->refcnt);
    80004644:	415c                	lw	a5,4(a0)
    80004646:	0007871b          	sext.w	a4,a5
    8000464a:	1782                	slli	a5,a5,0x20
    8000464c:	9381                	srli	a5,a5,0x20
    if (old_cnt == 0)
    8000464e:	cf81                	beqz	a5,80004666 <spike_file_get+0x44>
  } while (atomic_cas(&f->refcnt, old_cnt, old_cnt+1) != old_cnt);
    80004650:	415c                	lw	a5,4(a0)
    80004652:	2781                	sext.w	a5,a5
    80004654:	fef716e3          	bne	a4,a5,80004640 <spike_file_get+0x1e>
    80004658:	0017069b          	addiw	a3,a4,1
    8000465c:	c154                	sw	a3,4(a0)
    8000465e:	b7cd                	j	80004640 <spike_file_get+0x1e>
    80004660:	8082                	ret
    return 0;
    80004662:	4501                	li	a0,0
    80004664:	8082                	ret
      return 0;
    80004666:	4501                	li	a0,0

  return f;
}
    80004668:	8082                	ret

000000008000466a <htif_done>:
  }
}

static void htif_done(const struct fdt_scan_node *node, void *extra) {
  struct htif_scan *scan = (struct htif_scan *)extra;
  if (!scan->compat) return;
    8000466a:	419c                	lw	a5,0(a1)
    8000466c:	c791                	beqz	a5,80004678 <htif_done+0xe>

  htif = 1;
    8000466e:	4785                	li	a5,1
    80004670:	0000c717          	auipc	a4,0xc
    80004674:	aef73823          	sd	a5,-1296(a4) # 80010160 <htif>
}
    80004678:	8082                	ret

000000008000467a <htif_open>:
static void htif_open(const struct fdt_scan_node *node, void *extra) {
    8000467a:	1141                	addi	sp,sp,-16
    8000467c:	e406                	sd	ra,8(sp)
    8000467e:	852e                	mv	a0,a1
  memset(scan, 0, sizeof(*scan));
    80004680:	4611                	li	a2,4
    80004682:	4581                	li	a1,0
    80004684:	ad1ff0ef          	jal	80004154 <memset>
}
    80004688:	60a2                	ld	ra,8(sp)
    8000468a:	0141                	addi	sp,sp,16
    8000468c:	8082                	ret

000000008000468e <htif_prop>:
static void htif_prop(const struct fdt_scan_prop *prop, void *extra) {
    8000468e:	1101                	addi	sp,sp,-32
    80004690:	ec06                	sd	ra,24(sp)
    80004692:	e822                	sd	s0,16(sp)
    80004694:	e426                	sd	s1,8(sp)
    80004696:	842a                	mv	s0,a0
    80004698:	84ae                	mv	s1,a1
  if (!strcmp(prop->name, "compatible") && !strcmp((const char *)prop->value, "ucb,htif0")) {
    8000469a:	00004597          	auipc	a1,0x4
    8000469e:	67658593          	addi	a1,a1,1654 # 80008d10 <rfs_i_ops+0x1c98>
    800046a2:	6508                	ld	a0,8(a0)
    800046a4:	b07ff0ef          	jal	800041aa <strcmp>
    800046a8:	c511                	beqz	a0,800046b4 <htif_prop+0x26>
}
    800046aa:	60e2                	ld	ra,24(sp)
    800046ac:	6442                	ld	s0,16(sp)
    800046ae:	64a2                	ld	s1,8(sp)
    800046b0:	6105                	addi	sp,sp,32
    800046b2:	8082                	ret
  if (!strcmp(prop->name, "compatible") && !strcmp((const char *)prop->value, "ucb,htif0")) {
    800046b4:	00004597          	auipc	a1,0x4
    800046b8:	66c58593          	addi	a1,a1,1644 # 80008d20 <rfs_i_ops+0x1ca8>
    800046bc:	6808                	ld	a0,16(s0)
    800046be:	aedff0ef          	jal	800041aa <strcmp>
    800046c2:	f565                	bnez	a0,800046aa <htif_prop+0x1c>
    scan->compat = 1;
    800046c4:	4785                	li	a5,1
    800046c6:	c09c                	sw	a5,0(s1)
}
    800046c8:	b7cd                	j	800046aa <htif_prop+0x1c>

00000000800046ca <__check_fromhost>:

volatile int htif_console_buf;
static spinlock_t htif_lock = SPINLOCK_INIT;

static void __check_fromhost(void) {
  uint64_t fh = fromhost;
    800046ca:	00005797          	auipc	a5,0x5
    800046ce:	9367b783          	ld	a5,-1738(a5) # 80009000 <fromhost>
  if (!fh) return;
    800046d2:	c7d1                	beqz	a5,8000475e <__check_fromhost+0x94>
static void __check_fromhost(void) {
    800046d4:	1141                	addi	sp,sp,-16
    800046d6:	e406                	sd	ra,8(sp)
  fromhost = 0;
    800046d8:	00005717          	auipc	a4,0x5
    800046dc:	92073423          	sd	zero,-1752(a4) # 80009000 <fromhost>

  // this should be from the console
  assert(FROMHOST_DEV(fh) == 1);
    800046e0:	0387d713          	srli	a4,a5,0x38
    800046e4:	4685                	li	a3,1
    800046e6:	00d71c63          	bne	a4,a3,800046fe <__check_fromhost+0x34>
  switch (FROMHOST_CMD(fh)) {
    800046ea:	00879713          	slli	a4,a5,0x8
    800046ee:	9361                	srli	a4,a4,0x38
    800046f0:	cb1d                	beqz	a4,80004726 <__check_fromhost+0x5c>
    800046f2:	4785                	li	a5,1
    800046f4:	04f71163          	bne	a4,a5,80004736 <__check_fromhost+0x6c>
    case 1:
      break;
    default:
      assert(0);
  }
}
    800046f8:	60a2                	ld	ra,8(sp)
    800046fa:	0141                	addi	sp,sp,16
    800046fc:	8082                	ret
  assert(FROMHOST_DEV(fh) == 1);
    800046fe:	00004697          	auipc	a3,0x4
    80004702:	63268693          	addi	a3,a3,1586 # 80008d30 <rfs_i_ops+0x1cb8>
    80004706:	05100613          	li	a2,81
    8000470a:	00004597          	auipc	a1,0x4
    8000470e:	63e58593          	addi	a1,a1,1598 # 80008d48 <rfs_i_ops+0x1cd0>
    80004712:	00003517          	auipc	a0,0x3
    80004716:	07650513          	addi	a0,a0,118 # 80007788 <rfs_i_ops+0x710>
    8000471a:	4d8000ef          	jal	80004bf2 <sprint>
    8000471e:	6541                	lui	a0,0x10
    80004720:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    80004722:	4f0000ef          	jal	80004c12 <poweroff>
      htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
    80004726:	0ff7f793          	zext.b	a5,a5
    8000472a:	2785                	addiw	a5,a5,1
    8000472c:	0000c717          	auipc	a4,0xc
    80004730:	a2f72623          	sw	a5,-1492(a4) # 80010158 <htif_console_buf>
      break;
    80004734:	b7d1                	j	800046f8 <__check_fromhost+0x2e>
      assert(0);
    80004736:	00004697          	auipc	a3,0x4
    8000473a:	65a68693          	addi	a3,a3,1626 # 80008d90 <rfs_i_ops+0x1d18>
    8000473e:	05900613          	li	a2,89
    80004742:	00004597          	auipc	a1,0x4
    80004746:	60658593          	addi	a1,a1,1542 # 80008d48 <rfs_i_ops+0x1cd0>
    8000474a:	00003517          	auipc	a0,0x3
    8000474e:	03e50513          	addi	a0,a0,62 # 80007788 <rfs_i_ops+0x710>
    80004752:	4a0000ef          	jal	80004bf2 <sprint>
    80004756:	6541                	lui	a0,0x10
    80004758:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    8000475a:	4b8000ef          	jal	80004c12 <poweroff>
    8000475e:	8082                	ret

0000000080004760 <__set_tohost>:

static void __set_tohost(uint64 dev, uint64 cmd, uint64 data) {
    80004760:	1101                	addi	sp,sp,-32
    80004762:	ec06                	sd	ra,24(sp)
    80004764:	e822                	sd	s0,16(sp)
    80004766:	e426                	sd	s1,8(sp)
    80004768:	e04a                	sd	s2,0(sp)
    8000476a:	842a                	mv	s0,a0
    8000476c:	84ae                	mv	s1,a1
    8000476e:	8932                	mv	s2,a2
  while (tohost) __check_fromhost();
    80004770:	a019                	j	80004776 <__set_tohost+0x16>
    80004772:	f59ff0ef          	jal	800046ca <__check_fromhost>
    80004776:	00005797          	auipc	a5,0x5
    8000477a:	8927b783          	ld	a5,-1902(a5) # 80009008 <tohost>
    8000477e:	fbf5                	bnez	a5,80004772 <__set_tohost+0x12>
  tohost = TOHOST_CMD(dev, cmd, data);
    80004780:	1462                	slli	s0,s0,0x38
    80004782:	14c2                	slli	s1,s1,0x30
    80004784:	8c45                	or	s0,s0,s1
    80004786:	01246433          	or	s0,s0,s2
    8000478a:	00005797          	auipc	a5,0x5
    8000478e:	8687bf23          	sd	s0,-1922(a5) # 80009008 <tohost>
}
    80004792:	60e2                	ld	ra,24(sp)
    80004794:	6442                	ld	s0,16(sp)
    80004796:	64a2                	ld	s1,8(sp)
    80004798:	6902                	ld	s2,0(sp)
    8000479a:	6105                	addi	sp,sp,32
    8000479c:	8082                	ret

000000008000479e <do_tohost_fromhost>:

static void do_tohost_fromhost(uint64 dev, uint64 cmd, uint64 data) {
    8000479e:	1101                	addi	sp,sp,-32
    800047a0:	ec06                	sd	ra,24(sp)
    800047a2:	e822                	sd	s0,16(sp)
    800047a4:	e426                	sd	s1,8(sp)
    800047a6:	842a                	mv	s0,a0
    800047a8:	84ae                	mv	s1,a1
  return res;
}

static inline void spinlock_lock(spinlock_t* lock) {
  do {
    while (atomic_read(&lock->lock))
    800047aa:	0000c797          	auipc	a5,0xc
    800047ae:	8f67a783          	lw	a5,-1802(a5) # 800100a0 <htif_lock>
    800047b2:	ffe5                	bnez	a5,800047aa <do_tohost_fromhost+0xc>
  int res = atomic_swap(&lock->lock, -1);
    800047b4:	0000c717          	auipc	a4,0xc
    800047b8:	8ec70713          	addi	a4,a4,-1812 # 800100a0 <htif_lock>
    800047bc:	431c                	lw	a5,0(a4)
    800047be:	2781                	sext.w	a5,a5
    800047c0:	56fd                	li	a3,-1
    800047c2:	c314                	sw	a3,0(a4)
  mb();
    800047c4:	0ff0000f          	fence
      ;
  } while (spinlock_trylock(lock));
    800047c8:	f3ed                	bnez	a5,800047aa <do_tohost_fromhost+0xc>
  spinlock_lock(&htif_lock);
  __set_tohost(dev, cmd, data);
    800047ca:	85a6                	mv	a1,s1
    800047cc:	8522                	mv	a0,s0
    800047ce:	f93ff0ef          	jal	80004760 <__set_tohost>
    800047d2:	a019                	j	800047d8 <do_tohost_fromhost+0x3a>
    if (fh) {
      if (FROMHOST_DEV(fh) == dev && FROMHOST_CMD(fh) == cmd) {
        fromhost = 0;
        break;
      }
      __check_fromhost();
    800047d4:	ef7ff0ef          	jal	800046ca <__check_fromhost>
    uint64_t fh = fromhost;
    800047d8:	00005797          	auipc	a5,0x5
    800047dc:	8287b783          	ld	a5,-2008(a5) # 80009000 <fromhost>
    if (fh) {
    800047e0:	dfe5                	beqz	a5,800047d8 <do_tohost_fromhost+0x3a>
      if (FROMHOST_DEV(fh) == dev && FROMHOST_CMD(fh) == cmd) {
    800047e2:	0387d713          	srli	a4,a5,0x38
    800047e6:	fe8717e3          	bne	a4,s0,800047d4 <do_tohost_fromhost+0x36>
    800047ea:	07a2                	slli	a5,a5,0x8
    800047ec:	93e1                	srli	a5,a5,0x38
    800047ee:	fe9793e3          	bne	a5,s1,800047d4 <do_tohost_fromhost+0x36>
        fromhost = 0;
    800047f2:	00005797          	auipc	a5,0x5
    800047f6:	8007b723          	sd	zero,-2034(a5) # 80009000 <fromhost>
}

static inline void spinlock_unlock(spinlock_t* lock) {
  mb();
    800047fa:	0ff0000f          	fence
  atomic_set(&lock->lock, 0);
    800047fe:	0000c797          	auipc	a5,0xc
    80004802:	8a07a123          	sw	zero,-1886(a5) # 800100a0 <htif_lock>
    }
  }
  spinlock_unlock(&htif_lock);
}
    80004806:	60e2                	ld	ra,24(sp)
    80004808:	6442                	ld	s0,16(sp)
    8000480a:	64a2                	ld	s1,8(sp)
    8000480c:	6105                	addi	sp,sp,32
    8000480e:	8082                	ret

0000000080004810 <query_htif>:
void query_htif(uint64 fdt) {
    80004810:	7139                	addi	sp,sp,-64
    80004812:	fc06                	sd	ra,56(sp)
    80004814:	f822                	sd	s0,48(sp)
    80004816:	842a                	mv	s0,a0
  memset(&cb, 0, sizeof(cb));
    80004818:	02800613          	li	a2,40
    8000481c:	4581                	li	a1,0
    8000481e:	0028                	addi	a0,sp,8
    80004820:	935ff0ef          	jal	80004154 <memset>
  cb.open = htif_open;
    80004824:	00000797          	auipc	a5,0x0
    80004828:	e5678793          	addi	a5,a5,-426 # 8000467a <htif_open>
    8000482c:	e43e                	sd	a5,8(sp)
  cb.prop = htif_prop;
    8000482e:	00000797          	auipc	a5,0x0
    80004832:	e6078793          	addi	a5,a5,-416 # 8000468e <htif_prop>
    80004836:	e83e                	sd	a5,16(sp)
  cb.done = htif_done;
    80004838:	00000797          	auipc	a5,0x0
    8000483c:	e3278793          	addi	a5,a5,-462 # 8000466a <htif_done>
    80004840:	ec3e                	sd	a5,24(sp)
  cb.extra = &scan;
    80004842:	f40a                	sd	sp,40(sp)
  fdt_scan(fdt, &cb);
    80004844:	002c                	addi	a1,sp,8
    80004846:	8522                	mv	a0,s0
    80004848:	15b000ef          	jal	800051a2 <fdt_scan>
}
    8000484c:	70e2                	ld	ra,56(sp)
    8000484e:	7442                	ld	s0,48(sp)
    80004850:	6121                	addi	sp,sp,64
    80004852:	8082                	ret

0000000080004854 <htif_syscall>:

/////////////////////    Encapsulated Spike HTIF functionalities    //////////////////
void htif_syscall(uint64 arg) { do_tohost_fromhost(0, 0, arg); }
    80004854:	1141                	addi	sp,sp,-16
    80004856:	e406                	sd	ra,8(sp)
    80004858:	862a                	mv	a2,a0
    8000485a:	4581                	li	a1,0
    8000485c:	4501                	li	a0,0
    8000485e:	f41ff0ef          	jal	8000479e <do_tohost_fromhost>
    80004862:	60a2                	ld	ra,8(sp)
    80004864:	0141                	addi	sp,sp,16
    80004866:	8082                	ret

0000000080004868 <htif_console_putchar>:

// htif fuctionalities
void htif_console_putchar(uint8_t ch) {
    80004868:	1141                	addi	sp,sp,-16
    8000486a:	e406                	sd	ra,8(sp)
    8000486c:	862a                	mv	a2,a0
    while (atomic_read(&lock->lock))
    8000486e:	0000c797          	auipc	a5,0xc
    80004872:	8327a783          	lw	a5,-1998(a5) # 800100a0 <htif_lock>
    80004876:	ffe5                	bnez	a5,8000486e <htif_console_putchar+0x6>
  int res = atomic_swap(&lock->lock, -1);
    80004878:	0000c717          	auipc	a4,0xc
    8000487c:	82870713          	addi	a4,a4,-2008 # 800100a0 <htif_lock>
    80004880:	431c                	lw	a5,0(a4)
    80004882:	2781                	sext.w	a5,a5
    80004884:	56fd                	li	a3,-1
    80004886:	c314                	sw	a3,0(a4)
  mb();
    80004888:	0ff0000f          	fence
  } while (spinlock_trylock(lock));
    8000488c:	f3ed                	bnez	a5,8000486e <htif_console_putchar+0x6>
  magic_mem[2] = (uint64)&ch;
  magic_mem[3] = 1;
  do_tohost_fromhost(0, 0, (uint64)magic_mem);
#else
  spinlock_lock(&htif_lock);
  __set_tohost(1, 1, ch);
    8000488e:	4585                	li	a1,1
    80004890:	4505                	li	a0,1
    80004892:	ecfff0ef          	jal	80004760 <__set_tohost>
  mb();
    80004896:	0ff0000f          	fence
  atomic_set(&lock->lock, 0);
    8000489a:	0000c797          	auipc	a5,0xc
    8000489e:	8007a323          	sw	zero,-2042(a5) # 800100a0 <htif_lock>
  spinlock_unlock(&htif_lock);
#endif
}
    800048a2:	60a2                	ld	ra,8(sp)
    800048a4:	0141                	addi	sp,sp,16
    800048a6:	8082                	ret

00000000800048a8 <htif_console_getchar>:

int htif_console_getchar(void) {
    800048a8:	1141                	addi	sp,sp,-16
    800048aa:	e406                	sd	ra,8(sp)
    800048ac:	e022                	sd	s0,0(sp)
    while (atomic_read(&lock->lock))
    800048ae:	0000b797          	auipc	a5,0xb
    800048b2:	7f27a783          	lw	a5,2034(a5) # 800100a0 <htif_lock>
    800048b6:	ffe5                	bnez	a5,800048ae <htif_console_getchar+0x6>
  int res = atomic_swap(&lock->lock, -1);
    800048b8:	0000b717          	auipc	a4,0xb
    800048bc:	7e870713          	addi	a4,a4,2024 # 800100a0 <htif_lock>
    800048c0:	431c                	lw	a5,0(a4)
    800048c2:	2781                	sext.w	a5,a5
    800048c4:	56fd                	li	a3,-1
    800048c6:	c314                	sw	a3,0(a4)
  mb();
    800048c8:	0ff0000f          	fence
  } while (spinlock_trylock(lock));
    800048cc:	f3ed                	bnez	a5,800048ae <htif_console_getchar+0x6>
  // HTIF devices are not supported on RV32
  return -1;
#endif

  spinlock_lock(&htif_lock);
  __check_fromhost();
    800048ce:	dfdff0ef          	jal	800046ca <__check_fromhost>
  int ch = htif_console_buf;
    800048d2:	0000c417          	auipc	s0,0xc
    800048d6:	88642403          	lw	s0,-1914(s0) # 80010158 <htif_console_buf>
  if (ch >= 0) {
    800048da:	00045e63          	bgez	s0,800048f6 <htif_console_getchar+0x4e>
  mb();
    800048de:	0ff0000f          	fence
  atomic_set(&lock->lock, 0);
    800048e2:	0000b797          	auipc	a5,0xb
    800048e6:	7a07af23          	sw	zero,1982(a5) # 800100a0 <htif_lock>
    __set_tohost(1, 0, 0);
  }
  spinlock_unlock(&htif_lock);

  return ch - 1;
}
    800048ea:	fff4051b          	addiw	a0,s0,-1
    800048ee:	60a2                	ld	ra,8(sp)
    800048f0:	6402                	ld	s0,0(sp)
    800048f2:	0141                	addi	sp,sp,16
    800048f4:	8082                	ret
    htif_console_buf = -1;
    800048f6:	57fd                	li	a5,-1
    800048f8:	0000c717          	auipc	a4,0xc
    800048fc:	86f72023          	sw	a5,-1952(a4) # 80010158 <htif_console_buf>
    __set_tohost(1, 0, 0);
    80004900:	4601                	li	a2,0
    80004902:	4581                	li	a1,0
    80004904:	4505                	li	a0,1
    80004906:	e5bff0ef          	jal	80004760 <__set_tohost>
    8000490a:	bfd1                	j	800048de <htif_console_getchar+0x36>

000000008000490c <htif_poweroff>:

void htif_poweroff(void) {
  while (1) {
    fromhost = 0;
    8000490c:	00004797          	auipc	a5,0x4
    80004910:	6f478793          	addi	a5,a5,1780 # 80009000 <fromhost>
    80004914:	0007b023          	sd	zero,0(a5)
    tohost = 1;
    80004918:	4705                	li	a4,1
    8000491a:	e798                	sd	a4,8(a5)
  while (1) {
    8000491c:	bfc5                	j	8000490c <htif_poweroff>

000000008000491e <mem_open>:
  int memory;
  const uint32 *reg_value;
  int reg_len;
};

static void mem_open(const struct fdt_scan_node *node, void *extra) {
    8000491e:	1141                	addi	sp,sp,-16
    80004920:	e406                	sd	ra,8(sp)
    80004922:	852e                	mv	a0,a1
  struct mem_scan *scan = (struct mem_scan *)extra;
  memset(scan, 0, sizeof(*scan));
    80004924:	4661                	li	a2,24
    80004926:	4581                	li	a1,0
    80004928:	82dff0ef          	jal	80004154 <memset>
}
    8000492c:	60a2                	ld	ra,8(sp)
    8000492e:	0141                	addi	sp,sp,16
    80004930:	8082                	ret

0000000080004932 <mem_done>:
    scan->reg_value = prop->value;
    scan->reg_len = prop->len;
  }
}

static void mem_done(const struct fdt_scan_node *node, void *extra) {
    80004932:	7179                	addi	sp,sp,-48
    80004934:	f406                	sd	ra,40(sp)
    80004936:	f022                	sd	s0,32(sp)
    80004938:	87ae                	mv	a5,a1
  struct mem_scan *scan = (struct mem_scan *)extra;
  const uint32 *value = scan->reg_value;
    8000493a:	658c                	ld	a1,8(a1)
  const uint32 *end = value + scan->reg_len / 4;
    8000493c:	4b98                	lw	a4,16(a5)
    8000493e:	41f7541b          	sraiw	s0,a4,0x1f
    80004942:	01e4541b          	srliw	s0,s0,0x1e
    80004946:	9c39                	addw	s0,s0,a4
    80004948:	4024541b          	sraiw	s0,s0,0x2
    8000494c:	040a                	slli	s0,s0,0x2
    8000494e:	942e                	add	s0,s0,a1
  uint64 self = (uint64)mem_done;

  if (!scan->memory) return;
    80004950:	439c                	lw	a5,0(a5)
    80004952:	cfad                	beqz	a5,800049cc <mem_done+0x9a>
    80004954:	ec26                	sd	s1,24(sp)
    80004956:	e84a                	sd	s2,16(sp)
    80004958:	84aa                	mv	s1,a0
    8000495a:	00000917          	auipc	s2,0x0
    8000495e:	fd890913          	addi	s2,s2,-40 # 80004932 <mem_done>
  assert(scan->reg_value && scan->reg_len % 4 == 0);
    80004962:	cd8d                	beqz	a1,8000499c <mem_done+0x6a>
    80004964:	8b0d                	andi	a4,a4,3
    80004966:	eb1d                	bnez	a4,8000499c <mem_done+0x6a>

  while (end - value > 0) {
    80004968:	40b407b3          	sub	a5,s0,a1
    8000496c:	04f05c63          	blez	a5,800049c4 <mem_done+0x92>
    uint64 base, size;
    value = fdt_get_address(node->parent, value, &base);
    80004970:	860a                	mv	a2,sp
    80004972:	6088                	ld	a0,0(s1)
    80004974:	78a000ef          	jal	800050fe <fdt_get_address>
    80004978:	85aa                	mv	a1,a0
    value = fdt_get_size(node->parent, value, &size);
    8000497a:	0030                	addi	a2,sp,8
    8000497c:	6088                	ld	a0,0(s1)
    8000497e:	7d2000ef          	jal	80005150 <fdt_get_size>
    80004982:	85aa                	mv	a1,a0
    if (base <= self && self <= base + size) {
    80004984:	6782                	ld	a5,0(sp)
    80004986:	fef961e3          	bltu	s2,a5,80004968 <mem_done+0x36>
    8000498a:	6722                	ld	a4,8(sp)
    8000498c:	97ba                	add	a5,a5,a4
    8000498e:	fd27ede3          	bltu	a5,s2,80004968 <mem_done+0x36>
      g_mem_size = size;
    80004992:	0000b797          	auipc	a5,0xb
    80004996:	7ce7bb23          	sd	a4,2006(a5) # 80010168 <g_mem_size>
    8000499a:	b7f9                	j	80004968 <mem_done+0x36>
  assert(scan->reg_value && scan->reg_len % 4 == 0);
    8000499c:	00004697          	auipc	a3,0x4
    800049a0:	3cc68693          	addi	a3,a3,972 # 80008d68 <rfs_i_ops+0x1cf0>
    800049a4:	02900613          	li	a2,41
    800049a8:	00004597          	auipc	a1,0x4
    800049ac:	3f058593          	addi	a1,a1,1008 # 80008d98 <rfs_i_ops+0x1d20>
    800049b0:	00003517          	auipc	a0,0x3
    800049b4:	dd850513          	addi	a0,a0,-552 # 80007788 <rfs_i_ops+0x710>
    800049b8:	23a000ef          	jal	80004bf2 <sprint>
    800049bc:	6541                	lui	a0,0x10
    800049be:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    800049c0:	252000ef          	jal	80004c12 <poweroff>
    }
  }
  assert(end == value);
    800049c4:	00859863          	bne	a1,s0,800049d4 <mem_done+0xa2>
    800049c8:	64e2                	ld	s1,24(sp)
    800049ca:	6942                	ld	s2,16(sp)
}
    800049cc:	70a2                	ld	ra,40(sp)
    800049ce:	7402                	ld	s0,32(sp)
    800049d0:	6145                	addi	sp,sp,48
    800049d2:	8082                	ret
  assert(end == value);
    800049d4:	00004697          	auipc	a3,0x4
    800049d8:	3e468693          	addi	a3,a3,996 # 80008db8 <rfs_i_ops+0x1d40>
    800049dc:	03300613          	li	a2,51
    800049e0:	00004597          	auipc	a1,0x4
    800049e4:	3b858593          	addi	a1,a1,952 # 80008d98 <rfs_i_ops+0x1d20>
    800049e8:	00003517          	auipc	a0,0x3
    800049ec:	da050513          	addi	a0,a0,-608 # 80007788 <rfs_i_ops+0x710>
    800049f0:	202000ef          	jal	80004bf2 <sprint>
    800049f4:	6541                	lui	a0,0x10
    800049f6:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    800049f8:	21a000ef          	jal	80004c12 <poweroff>

00000000800049fc <mem_prop>:
static void mem_prop(const struct fdt_scan_prop *prop, void *extra) {
    800049fc:	1101                	addi	sp,sp,-32
    800049fe:	ec06                	sd	ra,24(sp)
    80004a00:	e822                	sd	s0,16(sp)
    80004a02:	e426                	sd	s1,8(sp)
    80004a04:	842a                	mv	s0,a0
    80004a06:	84ae                	mv	s1,a1
  if (!strcmp(prop->name, "device_type") && !strcmp((const char *)prop->value, "memory")) {
    80004a08:	00004597          	auipc	a1,0x4
    80004a0c:	3c058593          	addi	a1,a1,960 # 80008dc8 <rfs_i_ops+0x1d50>
    80004a10:	6508                	ld	a0,8(a0)
    80004a12:	f98ff0ef          	jal	800041aa <strcmp>
    80004a16:	ed01                	bnez	a0,80004a2e <mem_prop+0x32>
    80004a18:	00004597          	auipc	a1,0x4
    80004a1c:	3c058593          	addi	a1,a1,960 # 80008dd8 <rfs_i_ops+0x1d60>
    80004a20:	6808                	ld	a0,16(s0)
    80004a22:	f88ff0ef          	jal	800041aa <strcmp>
    80004a26:	e501                	bnez	a0,80004a2e <mem_prop+0x32>
    scan->memory = 1;
    80004a28:	4785                	li	a5,1
    80004a2a:	c09c                	sw	a5,0(s1)
    80004a2c:	a829                	j	80004a46 <mem_prop+0x4a>
  } else if (!strcmp(prop->name, "reg")) {
    80004a2e:	00004597          	auipc	a1,0x4
    80004a32:	3b258593          	addi	a1,a1,946 # 80008de0 <rfs_i_ops+0x1d68>
    80004a36:	6408                	ld	a0,8(s0)
    80004a38:	f72ff0ef          	jal	800041aa <strcmp>
    80004a3c:	e509                	bnez	a0,80004a46 <mem_prop+0x4a>
    scan->reg_value = prop->value;
    80004a3e:	681c                	ld	a5,16(s0)
    80004a40:	e49c                	sd	a5,8(s1)
    scan->reg_len = prop->len;
    80004a42:	4c1c                	lw	a5,24(s0)
    80004a44:	c89c                	sw	a5,16(s1)
}
    80004a46:	60e2                	ld	ra,24(sp)
    80004a48:	6442                	ld	s0,16(sp)
    80004a4a:	64a2                	ld	s1,8(sp)
    80004a4c:	6105                	addi	sp,sp,32
    80004a4e:	8082                	ret

0000000080004a50 <query_mem>:

// scanning the emulated memory
void query_mem(uint64 fdt) {
    80004a50:	711d                	addi	sp,sp,-96
    80004a52:	ec86                	sd	ra,88(sp)
    80004a54:	e8a2                	sd	s0,80(sp)
    80004a56:	e4a6                	sd	s1,72(sp)
    80004a58:	842a                	mv	s0,a0
  struct fdt_cb cb;
  struct mem_scan scan;

  memset(&cb, 0, sizeof(cb));
    80004a5a:	02800613          	li	a2,40
    80004a5e:	4581                	li	a1,0
    80004a60:	0828                	addi	a0,sp,24
    80004a62:	ef2ff0ef          	jal	80004154 <memset>
  cb.open = mem_open;
    80004a66:	00000797          	auipc	a5,0x0
    80004a6a:	eb878793          	addi	a5,a5,-328 # 8000491e <mem_open>
    80004a6e:	ec3e                	sd	a5,24(sp)
  cb.prop = mem_prop;
    80004a70:	00000797          	auipc	a5,0x0
    80004a74:	f8c78793          	addi	a5,a5,-116 # 800049fc <mem_prop>
    80004a78:	f03e                	sd	a5,32(sp)
  cb.done = mem_done;
    80004a7a:	00000797          	auipc	a5,0x0
    80004a7e:	eb878793          	addi	a5,a5,-328 # 80004932 <mem_done>
    80004a82:	f43e                	sd	a5,40(sp)
  cb.extra = &scan;
    80004a84:	fc0a                	sd	sp,56(sp)

  g_mem_size = 0;
    80004a86:	0000b497          	auipc	s1,0xb
    80004a8a:	6e248493          	addi	s1,s1,1762 # 80010168 <g_mem_size>
    80004a8e:	0004b023          	sd	zero,0(s1)
  fdt_scan(fdt, &cb);
    80004a92:	082c                	addi	a1,sp,24
    80004a94:	8522                	mv	a0,s0
    80004a96:	70c000ef          	jal	800051a2 <fdt_scan>
  assert(g_mem_size > 0);
    80004a9a:	609c                	ld	a5,0(s1)
    80004a9c:	c791                	beqz	a5,80004aa8 <query_mem+0x58>
}
    80004a9e:	60e6                	ld	ra,88(sp)
    80004aa0:	6446                	ld	s0,80(sp)
    80004aa2:	64a6                	ld	s1,72(sp)
    80004aa4:	6125                	addi	sp,sp,96
    80004aa6:	8082                	ret
  assert(g_mem_size > 0);
    80004aa8:	00004697          	auipc	a3,0x4
    80004aac:	34068693          	addi	a3,a3,832 # 80008de8 <rfs_i_ops+0x1d70>
    80004ab0:	04300613          	li	a2,67
    80004ab4:	00004597          	auipc	a1,0x4
    80004ab8:	2e458593          	addi	a1,a1,740 # 80008d98 <rfs_i_ops+0x1d20>
    80004abc:	00003517          	auipc	a0,0x3
    80004ac0:	ccc50513          	addi	a0,a0,-820 # 80007788 <rfs_i_ops+0x710>
    80004ac4:	12e000ef          	jal	80004bf2 <sprint>
    80004ac8:	6541                	lui	a0,0x10
    80004aca:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    80004acc:	146000ef          	jal	80004c12 <poweroff>

0000000080004ad0 <mcall_console_putchar>:
  return ret;
}

//===============    Spike-assisted printf, output string to terminal    ===============
static uintptr_t mcall_console_putchar(uint8 ch) {
  if (htif) {
    80004ad0:	0000b797          	auipc	a5,0xb
    80004ad4:	6907b783          	ld	a5,1680(a5) # 80010160 <htif>
    80004ad8:	e399                	bnez	a5,80004ade <mcall_console_putchar+0xe>
    htif_console_putchar(ch);
  }
  return 0;
}
    80004ada:	4501                	li	a0,0
    80004adc:	8082                	ret
static uintptr_t mcall_console_putchar(uint8 ch) {
    80004ade:	1141                	addi	sp,sp,-16
    80004ae0:	e406                	sd	ra,8(sp)
    htif_console_putchar(ch);
    80004ae2:	d87ff0ef          	jal	80004868 <htif_console_putchar>
}
    80004ae6:	4501                	li	a0,0
    80004ae8:	60a2                	ld	ra,8(sp)
    80004aea:	0141                	addi	sp,sp,16
    80004aec:	8082                	ret

0000000080004aee <frontend_syscall>:
      uint64 a5, uint64 a6) {
    80004aee:	1141                	addi	sp,sp,-16
    80004af0:	e406                	sd	ra,8(sp)
    80004af2:	e022                	sd	s0,0(sp)
    while (atomic_read(&lock->lock))
    80004af4:	0000b317          	auipc	t1,0xb
    80004af8:	5c432303          	lw	t1,1476(t1) # 800100b8 <lock.1>
    80004afc:	fe031ce3          	bnez	t1,80004af4 <frontend_syscall+0x6>
  int res = atomic_swap(&lock->lock, -1);
    80004b00:	0000be17          	auipc	t3,0xb
    80004b04:	5b8e0e13          	addi	t3,t3,1464 # 800100b8 <lock.1>
    80004b08:	000e2303          	lw	t1,0(t3)
    80004b0c:	2301                	sext.w	t1,t1
    80004b0e:	5efd                	li	t4,-1
    80004b10:	01de2023          	sw	t4,0(t3)
  mb();
    80004b14:	0ff0000f          	fence
  } while (spinlock_trylock(lock));
    80004b18:	fc031ee3          	bnez	t1,80004af4 <frontend_syscall+0x6>
  magic_mem[0] = n;
    80004b1c:	8472                	mv	s0,t3
    80004b1e:	00ae3c23          	sd	a0,24(t3)
  magic_mem[1] = a0;
    80004b22:	02be3023          	sd	a1,32(t3)
  magic_mem[2] = a1;
    80004b26:	02ce3423          	sd	a2,40(t3)
  magic_mem[3] = a2;
    80004b2a:	02de3823          	sd	a3,48(t3)
  magic_mem[4] = a3;
    80004b2e:	02ee3c23          	sd	a4,56(t3)
  magic_mem[5] = a4;
    80004b32:	04fe3023          	sd	a5,64(t3)
  magic_mem[6] = a5;
    80004b36:	050e3423          	sd	a6,72(t3)
  magic_mem[7] = a6;
    80004b3a:	051e3823          	sd	a7,80(t3)
  htif_syscall((uintptr_t)magic_mem);
    80004b3e:	0000b517          	auipc	a0,0xb
    80004b42:	59250513          	addi	a0,a0,1426 # 800100d0 <magic_mem.0>
    80004b46:	d0fff0ef          	jal	80004854 <htif_syscall>
  long ret = magic_mem[0];
    80004b4a:	6c08                	ld	a0,24(s0)
  mb();
    80004b4c:	0ff0000f          	fence
  atomic_set(&lock->lock, 0);
    80004b50:	00042023          	sw	zero,0(s0)
}
    80004b54:	60a2                	ld	ra,8(sp)
    80004b56:	6402                	ld	s0,0(sp)
    80004b58:	0141                	addi	sp,sp,16
    80004b5a:	8082                	ret

0000000080004b5c <vprintk>:

void vprintk(const char* s, va_list vl) {
    80004b5c:	716d                	addi	sp,sp,-272
    80004b5e:	e606                	sd	ra,264(sp)
    80004b60:	862a                	mv	a2,a0
    80004b62:	86ae                	mv	a3,a1
  char out[256];
  int res = vsnprintf(out, sizeof(out), s, vl);
    80004b64:	10000593          	li	a1,256
    80004b68:	850a                	mv	a0,sp
    80004b6a:	160000ef          	jal	80004cca <vsnprintf>
  //you need spike_file_init before this call
  spike_file_write(stderr, out, res < sizeof(out) ? res : sizeof(out));
    80004b6e:	0005071b          	sext.w	a4,a0
    80004b72:	0ff00793          	li	a5,255
    80004b76:	00e7ed63          	bltu	a5,a4,80004b90 <vprintk+0x34>
    80004b7a:	862a                	mv	a2,a0
    80004b7c:	858a                	mv	a1,sp
    80004b7e:	00005517          	auipc	a0,0x5
    80004b82:	49250513          	addi	a0,a0,1170 # 8000a010 <spike_files+0x10>
    80004b86:	935ff0ef          	jal	800044ba <spike_file_write>
}
    80004b8a:	60b2                	ld	ra,264(sp)
    80004b8c:	6151                	addi	sp,sp,272
    80004b8e:	8082                	ret
  spike_file_write(stderr, out, res < sizeof(out) ? res : sizeof(out));
    80004b90:	10000613          	li	a2,256
    80004b94:	b7e5                	j	80004b7c <vprintk+0x20>

0000000080004b96 <printk>:

void printk(const char* s, ...) {
    80004b96:	711d                	addi	sp,sp,-96
    80004b98:	ec06                	sd	ra,24(sp)
    80004b9a:	f42e                	sd	a1,40(sp)
    80004b9c:	f832                	sd	a2,48(sp)
    80004b9e:	fc36                	sd	a3,56(sp)
    80004ba0:	e0ba                	sd	a4,64(sp)
    80004ba2:	e4be                	sd	a5,72(sp)
    80004ba4:	e8c2                	sd	a6,80(sp)
    80004ba6:	ecc6                	sd	a7,88(sp)
  va_list vl;
  va_start(vl, s);
    80004ba8:	102c                	addi	a1,sp,40
    80004baa:	e42e                	sd	a1,8(sp)

  vprintk(s, vl);
    80004bac:	fb1ff0ef          	jal	80004b5c <vprintk>

  va_end(vl);
}
    80004bb0:	60e2                	ld	ra,24(sp)
    80004bb2:	6125                	addi	sp,sp,96
    80004bb4:	8082                	ret

0000000080004bb6 <putstring>:

void putstring(const char* s) {
    80004bb6:	1141                	addi	sp,sp,-16
    80004bb8:	e406                	sd	ra,8(sp)
    80004bba:	e022                	sd	s0,0(sp)
    80004bbc:	842a                	mv	s0,a0
  while (*s) mcall_console_putchar(*s++);
    80004bbe:	a021                	j	80004bc6 <putstring+0x10>
    80004bc0:	0405                	addi	s0,s0,1
    80004bc2:	f0fff0ef          	jal	80004ad0 <mcall_console_putchar>
    80004bc6:	00044503          	lbu	a0,0(s0)
    80004bca:	f97d                	bnez	a0,80004bc0 <putstring+0xa>
}
    80004bcc:	60a2                	ld	ra,8(sp)
    80004bce:	6402                	ld	s0,0(sp)
    80004bd0:	0141                	addi	sp,sp,16
    80004bd2:	8082                	ret

0000000080004bd4 <vprintm>:

void vprintm(const char* s, va_list vl) {
    80004bd4:	716d                	addi	sp,sp,-272
    80004bd6:	e606                	sd	ra,264(sp)
    80004bd8:	862a                	mv	a2,a0
    80004bda:	86ae                	mv	a3,a1
  char buf[256];
  vsnprintf(buf, sizeof buf, s, vl);
    80004bdc:	10000593          	li	a1,256
    80004be0:	850a                	mv	a0,sp
    80004be2:	0e8000ef          	jal	80004cca <vsnprintf>
  putstring(buf);
    80004be6:	850a                	mv	a0,sp
    80004be8:	fcfff0ef          	jal	80004bb6 <putstring>
}
    80004bec:	60b2                	ld	ra,264(sp)
    80004bee:	6151                	addi	sp,sp,272
    80004bf0:	8082                	ret

0000000080004bf2 <sprint>:

void sprint(const char* s, ...) {
    80004bf2:	711d                	addi	sp,sp,-96
    80004bf4:	ec06                	sd	ra,24(sp)
    80004bf6:	f42e                	sd	a1,40(sp)
    80004bf8:	f832                	sd	a2,48(sp)
    80004bfa:	fc36                	sd	a3,56(sp)
    80004bfc:	e0ba                	sd	a4,64(sp)
    80004bfe:	e4be                	sd	a5,72(sp)
    80004c00:	e8c2                	sd	a6,80(sp)
    80004c02:	ecc6                	sd	a7,88(sp)
  va_list vl;
  va_start(vl, s);
    80004c04:	102c                	addi	a1,sp,40
    80004c06:	e42e                	sd	a1,8(sp)

  vprintk(s, vl);
    80004c08:	f55ff0ef          	jal	80004b5c <vprintk>

  va_end(vl);
}
    80004c0c:	60e2                	ld	ra,24(sp)
    80004c0e:	6125                	addi	sp,sp,96
    80004c10:	8082                	ret

0000000080004c12 <poweroff>:

//===============    Spike-assisted termination, panic and assert    ===============
void poweroff(uint16_t code) {
    80004c12:	1141                	addi	sp,sp,-16
    80004c14:	e406                	sd	ra,8(sp)
  assert(htif);
    80004c16:	0000b797          	auipc	a5,0xb
    80004c1a:	54a7b783          	ld	a5,1354(a5) # 80010160 <htif>
    80004c1e:	e78d                	bnez	a5,80004c48 <poweroff+0x36>
    80004c20:	00004697          	auipc	a3,0x4
    80004c24:	1d868693          	addi	a3,a3,472 # 80008df8 <rfs_i_ops+0x1d80>
    80004c28:	05400613          	li	a2,84
    80004c2c:	00004597          	auipc	a1,0x4
    80004c30:	1d458593          	addi	a1,a1,468 # 80008e00 <rfs_i_ops+0x1d88>
    80004c34:	00003517          	auipc	a0,0x3
    80004c38:	b5450513          	addi	a0,a0,-1196 # 80007788 <rfs_i_ops+0x710>
    80004c3c:	fb7ff0ef          	jal	80004bf2 <sprint>
    80004c40:	6541                	lui	a0,0x10
    80004c42:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    80004c44:	fcfff0ef          	jal	80004c12 <poweroff>
  sprint("Power off\r\n");
    80004c48:	00004517          	auipc	a0,0x4
    80004c4c:	1d850513          	addi	a0,a0,472 # 80008e20 <rfs_i_ops+0x1da8>
    80004c50:	fa3ff0ef          	jal	80004bf2 <sprint>
  if (htif) {
    80004c54:	0000b797          	auipc	a5,0xb
    80004c58:	50c7b783          	ld	a5,1292(a5) # 80010160 <htif>
    80004c5c:	e781                	bnez	a5,80004c64 <poweroff+0x52>
    htif_poweroff();
  } else {
    // we consider only one HART case in PKE experiments. May extend this later.
    // send_ipi_many(0, IPI_HALT);
    while (1) {
      asm volatile("wfi\n");
    80004c5e:	10500073          	wfi
    while (1) {
    80004c62:	bff5                	j	80004c5e <poweroff+0x4c>
    htif_poweroff();
    80004c64:	ca9ff0ef          	jal	8000490c <htif_poweroff>

0000000080004c68 <shutdown>:
    }
  }
}

void shutdown(int code) {
    80004c68:	1141                	addi	sp,sp,-16
    80004c6a:	e406                	sd	ra,8(sp)
    80004c6c:	e022                	sd	s0,0(sp)
    80004c6e:	842a                	mv	s0,a0
  sprint("System is shutting down with exit code %d.\n", code);
    80004c70:	85aa                	mv	a1,a0
    80004c72:	00004517          	auipc	a0,0x4
    80004c76:	1be50513          	addi	a0,a0,446 # 80008e30 <rfs_i_ops+0x1db8>
    80004c7a:	f79ff0ef          	jal	80004bf2 <sprint>
  frontend_syscall(HTIFSYS_exit, code, 0, 0, 0, 0, 0, 0);
    80004c7e:	4881                	li	a7,0
    80004c80:	4801                	li	a6,0
    80004c82:	4781                	li	a5,0
    80004c84:	4701                	li	a4,0
    80004c86:	4681                	li	a3,0
    80004c88:	4601                	li	a2,0
    80004c8a:	85a2                	mv	a1,s0
    80004c8c:	05d00513          	li	a0,93
    80004c90:	e5fff0ef          	jal	80004aee <frontend_syscall>
  while (1)
    80004c94:	a001                	j	80004c94 <shutdown+0x2c>

0000000080004c96 <do_panic>:
    ;
}

void do_panic(const char* s, ...) {
    80004c96:	711d                	addi	sp,sp,-96
    80004c98:	ec06                	sd	ra,24(sp)
    80004c9a:	f42e                	sd	a1,40(sp)
    80004c9c:	f832                	sd	a2,48(sp)
    80004c9e:	fc36                	sd	a3,56(sp)
    80004ca0:	e0ba                	sd	a4,64(sp)
    80004ca2:	e4be                	sd	a5,72(sp)
    80004ca4:	e8c2                	sd	a6,80(sp)
    80004ca6:	ecc6                	sd	a7,88(sp)
  va_list vl;
  va_start(vl, s);
    80004ca8:	102c                	addi	a1,sp,40
    80004caa:	e42e                	sd	a1,8(sp)

  sprint(s, vl);
    80004cac:	f47ff0ef          	jal	80004bf2 <sprint>
  shutdown(-1);
    80004cb0:	557d                	li	a0,-1
    80004cb2:	fb7ff0ef          	jal	80004c68 <shutdown>

0000000080004cb6 <kassert_fail>:

  va_end(vl);
}

void kassert_fail(const char* s) {
    80004cb6:	1141                	addi	sp,sp,-16
    80004cb8:	e406                	sd	ra,8(sp)
    80004cba:	862a                	mv	a2,a0
  register uintptr_t ra asm("ra");
  do_panic("assertion failed @ %p: %s\n", ra, s);
    80004cbc:	8586                	mv	a1,ra
    80004cbe:	00004517          	auipc	a0,0x4
    80004cc2:	1a250513          	addi	a0,a0,418 # 80008e60 <rfs_i_ops+0x1de8>
    80004cc6:	fd1ff0ef          	jal	80004c96 <do_panic>

0000000080004cca <vsnprintf>:
//#include <stdarg.h>
//#include <stdbool.h>

#include "util/snprintf.h"

int32 vsnprintf(char* out, size_t n, const char* s, va_list vl) {
    80004cca:	1141                	addi	sp,sp,-16
    80004ccc:	e436                	sd	a3,8(sp)
  bool format = FALSE;
  bool longarg = FALSE;
  size_t pos = 0;
    80004cce:	4781                	li	a5,0
  bool longarg = FALSE;
    80004cd0:	4301                	li	t1,0
  bool format = FALSE;
    80004cd2:	4681                	li	a3,0

  for (; *s; s++) {
    80004cd4:	aa49                	j	80004e66 <vsnprintf+0x19c>
        case 'l':
          longarg = TRUE;
          break;
        case 'p':
          longarg = TRUE;
          if (++pos < n) out[pos - 1] = '0';
    80004cd6:	00178713          	addi	a4,a5,1
    80004cda:	00b77863          	bgeu	a4,a1,80004cea <vsnprintf+0x20>
    80004cde:	00f506b3          	add	a3,a0,a5
    80004ce2:	03000813          	li	a6,48
    80004ce6:	01068023          	sb	a6,0(a3)
          if (++pos < n) out[pos - 1] = 'x';
    80004cea:	0789                	addi	a5,a5,2
    80004cec:	00b7ea63          	bltu	a5,a1,80004d00 <vsnprintf+0x36>
        case 'x': {
          long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
    80004cf0:	6722                	ld	a4,8(sp)
    80004cf2:	00870693          	addi	a3,a4,8
    80004cf6:	e436                	sd	a3,8(sp)
    80004cf8:	00073883          	ld	a7,0(a4)
          for (int i = 2 * (longarg ? sizeof(long) : sizeof(int)) - 1; i >= 0; i--) {
    80004cfc:	46bd                	li	a3,15
    80004cfe:	a83d                	j	80004d3c <vsnprintf+0x72>
          if (++pos < n) out[pos - 1] = 'x';
    80004d00:	972a                	add	a4,a4,a0
    80004d02:	07800693          	li	a3,120
    80004d06:	00d70023          	sb	a3,0(a4)
          long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
    80004d0a:	6722                	ld	a4,8(sp)
    80004d0c:	00870693          	addi	a3,a4,8
    80004d10:	e436                	sd	a3,8(sp)
    80004d12:	00073883          	ld	a7,0(a4)
          for (int i = 2 * (longarg ? sizeof(long) : sizeof(int)) - 1; i >= 0; i--) {
    80004d16:	46bd                	li	a3,15
    80004d18:	a015                	j	80004d3c <vsnprintf+0x72>
          long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
    80004d1a:	fe0318e3          	bnez	t1,80004d0a <vsnprintf+0x40>
    80004d1e:	6722                	ld	a4,8(sp)
    80004d20:	00870693          	addi	a3,a4,8
    80004d24:	e436                	sd	a3,8(sp)
    80004d26:	00072883          	lw	a7,0(a4)
          for (int i = 2 * (longarg ? sizeof(long) : sizeof(int)) - 1; i >= 0; i--) {
    80004d2a:	469d                	li	a3,7
    80004d2c:	a801                	j	80004d3c <vsnprintf+0x72>
            int d = (num >> (4 * i)) & 0xF;
            if (++pos < n) out[pos - 1] = (d < 10 ? '0' + d : 'a' + d - 10);
    80004d2e:	05770713          	addi	a4,a4,87
    80004d32:	97aa                	add	a5,a5,a0
    80004d34:	00e78023          	sb	a4,0(a5)
          for (int i = 2 * (longarg ? sizeof(long) : sizeof(int)) - 1; i >= 0; i--) {
    80004d38:	36fd                	addiw	a3,a3,-1
            if (++pos < n) out[pos - 1] = (d < 10 ? '0' + d : 'a' + d - 10);
    80004d3a:	87c2                	mv	a5,a6
          for (int i = 2 * (longarg ? sizeof(long) : sizeof(int)) - 1; i >= 0; i--) {
    80004d3c:	0206c163          	bltz	a3,80004d5e <vsnprintf+0x94>
            int d = (num >> (4 * i)) & 0xF;
    80004d40:	0026971b          	slliw	a4,a3,0x2
    80004d44:	40e8d733          	sra	a4,a7,a4
    80004d48:	8b3d                	andi	a4,a4,15
            if (++pos < n) out[pos - 1] = (d < 10 ? '0' + d : 'a' + d - 10);
    80004d4a:	00178813          	addi	a6,a5,1
    80004d4e:	feb875e3          	bgeu	a6,a1,80004d38 <vsnprintf+0x6e>
    80004d52:	4325                	li	t1,9
    80004d54:	fce34de3          	blt	t1,a4,80004d2e <vsnprintf+0x64>
    80004d58:	03070713          	addi	a4,a4,48
    80004d5c:	bfd9                	j	80004d32 <vsnprintf+0x68>
          }
          longarg = FALSE;
    80004d5e:	4301                	li	t1,0
          format = FALSE;
    80004d60:	4681                	li	a3,0
    80004d62:	a209                	j	80004e64 <vsnprintf+0x19a>
          break;
        }
        case 'd': {
          long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
    80004d64:	02030263          	beqz	t1,80004d88 <vsnprintf+0xbe>
    80004d68:	6722                	ld	a4,8(sp)
    80004d6a:	00870693          	addi	a3,a4,8
    80004d6e:	e436                	sd	a3,8(sp)
    80004d70:	00073883          	ld	a7,0(a4)
          if (num < 0) {
    80004d74:	0208c163          	bltz	a7,80004d96 <vsnprintf+0xcc>
            num = -num;
            if (++pos < n) out[pos - 1] = '-';
          }
          long digits = 1;
          for (long nn = num; nn /= 10; digits++)
    80004d78:	8746                	mv	a4,a7
          long digits = 1;
    80004d7a:	4305                	li	t1,1
          for (long nn = num; nn /= 10; digits++)
    80004d7c:	46a9                	li	a3,10
    80004d7e:	02d74733          	div	a4,a4,a3
    80004d82:	cb0d                	beqz	a4,80004db4 <vsnprintf+0xea>
    80004d84:	0305                	addi	t1,t1,1
    80004d86:	bfdd                	j	80004d7c <vsnprintf+0xb2>
          long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
    80004d88:	6722                	ld	a4,8(sp)
    80004d8a:	00870693          	addi	a3,a4,8
    80004d8e:	e436                	sd	a3,8(sp)
    80004d90:	00072883          	lw	a7,0(a4)
    80004d94:	b7c5                	j	80004d74 <vsnprintf+0xaa>
            num = -num;
    80004d96:	411008b3          	neg	a7,a7
            if (++pos < n) out[pos - 1] = '-';
    80004d9a:	00178713          	addi	a4,a5,1
    80004d9e:	00b77963          	bgeu	a4,a1,80004db0 <vsnprintf+0xe6>
    80004da2:	97aa                	add	a5,a5,a0
    80004da4:	02d00693          	li	a3,45
    80004da8:	00d78023          	sb	a3,0(a5)
    80004dac:	87ba                	mv	a5,a4
    80004dae:	b7e9                	j	80004d78 <vsnprintf+0xae>
    80004db0:	87ba                	mv	a5,a4
    80004db2:	b7d9                	j	80004d78 <vsnprintf+0xae>
            ;
          for (int i = digits - 1; i >= 0; i--) {
    80004db4:	fff3071b          	addiw	a4,t1,-1
    80004db8:	a029                	j	80004dc2 <vsnprintf+0xf8>
            if (pos + i + 1 < n) out[pos + i] = '0' + (num % 10);
            num /= 10;
    80004dba:	46a9                	li	a3,10
    80004dbc:	02d8c8b3          	div	a7,a7,a3
          for (int i = digits - 1; i >= 0; i--) {
    80004dc0:	377d                	addiw	a4,a4,-1
    80004dc2:	02074163          	bltz	a4,80004de4 <vsnprintf+0x11a>
            if (pos + i + 1 < n) out[pos + i] = '0' + (num % 10);
    80004dc6:	00f706b3          	add	a3,a4,a5
    80004dca:	00168813          	addi	a6,a3,1
    80004dce:	feb876e3          	bgeu	a6,a1,80004dba <vsnprintf+0xf0>
    80004dd2:	4829                	li	a6,10
    80004dd4:	0308e833          	rem	a6,a7,a6
    80004dd8:	96aa                	add	a3,a3,a0
    80004dda:	0308081b          	addiw	a6,a6,48
    80004dde:	01068023          	sb	a6,0(a3)
    80004de2:	bfe1                	j	80004dba <vsnprintf+0xf0>
          }
          pos += digits;
    80004de4:	979a                	add	a5,a5,t1
          longarg = FALSE;
    80004de6:	4301                	li	t1,0
          format = FALSE;
    80004de8:	4681                	li	a3,0
          break;
    80004dea:	a8ad                	j	80004e64 <vsnprintf+0x19a>
        }
        case 's': {
          const char* s2 = va_arg(vl, const char*);
    80004dec:	6722                	ld	a4,8(sp)
    80004dee:	00870693          	addi	a3,a4,8
    80004df2:	e436                	sd	a3,8(sp)
    80004df4:	6318                	ld	a4,0(a4)
          while (*s2) {
    80004df6:	a019                	j	80004dfc <vsnprintf+0x132>
            if (++pos < n) out[pos - 1] = *s2;
            s2++;
    80004df8:	0705                	addi	a4,a4,1
            if (++pos < n) out[pos - 1] = *s2;
    80004dfa:	87b6                	mv	a5,a3
          while (*s2) {
    80004dfc:	00074803          	lbu	a6,0(a4)
    80004e00:	00080a63          	beqz	a6,80004e14 <vsnprintf+0x14a>
            if (++pos < n) out[pos - 1] = *s2;
    80004e04:	00178693          	addi	a3,a5,1
    80004e08:	feb6f8e3          	bgeu	a3,a1,80004df8 <vsnprintf+0x12e>
    80004e0c:	97aa                	add	a5,a5,a0
    80004e0e:	01078023          	sb	a6,0(a5)
    80004e12:	b7dd                	j	80004df8 <vsnprintf+0x12e>
          }
          longarg = FALSE;
    80004e14:	4301                	li	t1,0
          format = FALSE;
    80004e16:	4681                	li	a3,0
    80004e18:	a0b1                	j	80004e64 <vsnprintf+0x19a>
          break;
        }
        case 'c': {
          if (++pos < n) out[pos - 1] = (char)va_arg(vl, int);
    80004e1a:	00178713          	addi	a4,a5,1
    80004e1e:	02b77e63          	bgeu	a4,a1,80004e5a <vsnprintf+0x190>
    80004e22:	66a2                	ld	a3,8(sp)
    80004e24:	00868813          	addi	a6,a3,8
    80004e28:	e442                	sd	a6,8(sp)
    80004e2a:	97aa                	add	a5,a5,a0
    80004e2c:	0006c683          	lbu	a3,0(a3)
    80004e30:	00d78023          	sb	a3,0(a5)
    80004e34:	87ba                	mv	a5,a4
          longarg = FALSE;
    80004e36:	4301                	li	t1,0
          format = FALSE;
    80004e38:	4681                	li	a3,0
    80004e3a:	a02d                	j	80004e64 <vsnprintf+0x19a>
          break;
        }
        default:
          break;
      }
    } else if (*s == '%')
    80004e3c:	02500813          	li	a6,37
    80004e40:	03070163          	beq	a4,a6,80004e62 <vsnprintf+0x198>
      format = TRUE;
    else if (++pos < n)
    80004e44:	00178813          	addi	a6,a5,1
    80004e48:	04b87463          	bgeu	a6,a1,80004e90 <vsnprintf+0x1c6>
      out[pos - 1] = *s;
    80004e4c:	97aa                	add	a5,a5,a0
    80004e4e:	00e78023          	sb	a4,0(a5)
    else if (++pos < n)
    80004e52:	87c2                	mv	a5,a6
    80004e54:	a801                	j	80004e64 <vsnprintf+0x19a>
          longarg = TRUE;
    80004e56:	8336                	mv	t1,a3
    80004e58:	a031                	j	80004e64 <vsnprintf+0x19a>
          if (++pos < n) out[pos - 1] = (char)va_arg(vl, int);
    80004e5a:	87ba                	mv	a5,a4
          longarg = FALSE;
    80004e5c:	4301                	li	t1,0
          format = FALSE;
    80004e5e:	4681                	li	a3,0
    80004e60:	a011                	j	80004e64 <vsnprintf+0x19a>
      format = TRUE;
    80004e62:	4685                	li	a3,1
  for (; *s; s++) {
    80004e64:	0605                	addi	a2,a2,1
    80004e66:	00064703          	lbu	a4,0(a2)
    80004e6a:	c70d                	beqz	a4,80004e94 <vsnprintf+0x1ca>
    if (format) {
    80004e6c:	dae1                	beqz	a3,80004e3c <vsnprintf+0x172>
      switch (*s) {
    80004e6e:	f9d7071b          	addiw	a4,a4,-99
    80004e72:	0ff77893          	zext.b	a7,a4
    80004e76:	4855                	li	a6,21
    80004e78:	ff1866e3          	bltu	a6,a7,80004e64 <vsnprintf+0x19a>
    80004e7c:	00289713          	slli	a4,a7,0x2
    80004e80:	00002817          	auipc	a6,0x2
    80004e84:	32480813          	addi	a6,a6,804 # 800071a4 <rfs_i_ops+0x12c>
    80004e88:	9742                	add	a4,a4,a6
    80004e8a:	4318                	lw	a4,0(a4)
    80004e8c:	9742                	add	a4,a4,a6
    80004e8e:	8702                	jr	a4
    else if (++pos < n)
    80004e90:	87c2                	mv	a5,a6
    80004e92:	bfc9                	j	80004e64 <vsnprintf+0x19a>
  }
  if (pos < n)
    80004e94:	00b7f963          	bgeu	a5,a1,80004ea6 <vsnprintf+0x1dc>
    out[pos] = 0;
    80004e98:	953e                	add	a0,a0,a5
    80004e9a:	00050023          	sb	zero,0(a0)
  else if (n)
    out[n - 1] = 0;
  return pos;
}
    80004e9e:	0007851b          	sext.w	a0,a5
    80004ea2:	0141                	addi	sp,sp,16
    80004ea4:	8082                	ret
  else if (n)
    80004ea6:	dde5                	beqz	a1,80004e9e <vsnprintf+0x1d4>
    out[n - 1] = 0;
    80004ea8:	15fd                	addi	a1,a1,-1
    80004eaa:	952e                	add	a0,a0,a1
    80004eac:	00050023          	sb	zero,0(a0)
    80004eb0:	b7fd                	j	80004e9e <vsnprintf+0x1d4>

0000000080004eb2 <fdt_scan_helper>:
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
  return z;
}

static uint32 *fdt_scan_helper(uint32 *lex, const char *strings, struct fdt_scan_node *node,
                               const struct fdt_cb *cb) {
    80004eb2:	7159                	addi	sp,sp,-112
    80004eb4:	f486                	sd	ra,104(sp)
    80004eb6:	f0a2                	sd	s0,96(sp)
    80004eb8:	eca6                	sd	s1,88(sp)
    80004eba:	e8ca                	sd	s2,80(sp)
    80004ebc:	e4ce                	sd	s3,72(sp)
    80004ebe:	e0d2                	sd	s4,64(sp)
    80004ec0:	842a                	mv	s0,a0
    80004ec2:	89ae                	mv	s3,a1
    80004ec4:	8932                	mv	s2,a2
    80004ec6:	84b6                	mv	s1,a3
  struct fdt_scan_node child;
  struct fdt_scan_prop prop;
  int last = 0;

  child.parent = node;
    80004ec8:	f432                	sd	a2,40(sp)
  // these are the default cell counts, as per the FDT spec
  child.address_cells = 2;
    80004eca:	4789                	li	a5,2
    80004ecc:	dc3e                	sw	a5,56(sp)
  child.size_cells = 1;
    80004ece:	4785                	li	a5,1
    80004ed0:	de3e                	sw	a5,60(sp)
  prop.node = node;
    80004ed2:	e432                	sd	a2,8(sp)
  int last = 0;
    80004ed4:	4a01                	li	s4,0

  while (1) {
    switch (bswap(lex[0])) {
    80004ed6:	401c                	lw	a5,0(s0)
  uint32 y = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    80004ed8:	0087971b          	slliw	a4,a5,0x8
    80004edc:	ff0106b7          	lui	a3,0xff010
    80004ee0:	f0068693          	addi	a3,a3,-256 # ffffffffff00ff00 <_end+0xffffffff7effef00>
    80004ee4:	8f75                	and	a4,a4,a3
    80004ee6:	2701                	sext.w	a4,a4
    80004ee8:	0087d69b          	srliw	a3,a5,0x8
    80004eec:	00ff07b7          	lui	a5,0xff0
    80004ef0:	0ff78793          	addi	a5,a5,255 # ff00ff <elf_fpread-0x7f00ff01>
    80004ef4:	8ff5                	and	a5,a5,a3
    80004ef6:	8fd9                	or	a5,a5,a4
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
    80004ef8:	0107971b          	slliw	a4,a5,0x10
    80004efc:	0107d79b          	srliw	a5,a5,0x10
    80004f00:	8fd9                	or	a5,a5,a4
    80004f02:	2781                	sext.w	a5,a5
    switch (bswap(lex[0])) {
    80004f04:	470d                	li	a4,3
    80004f06:	04e78563          	beq	a5,a4,80004f50 <fdt_scan_helper+0x9e>
    80004f0a:	02f76e63          	bltu	a4,a5,80004f46 <fdt_scan_helper+0x94>
    80004f0e:	4705                	li	a4,1
    80004f10:	16e78463          	beq	a5,a4,80005078 <fdt_scan_helper+0x1c6>
    80004f14:	4709                	li	a4,2
    80004f16:	1ce79363          	bne	a5,a4,800050dc <fdt_scan_helper+0x22a>
          while (lex != lex_next) *lex++ = bswap(FDT_NOP);
        lex = lex_next;
        break;
      }
      case FDT_END_NODE: {
        if (!last && node && cb->done) cb->done(node, cb->extra);
    80004f1a:	000a1963          	bnez	s4,80004f2c <fdt_scan_helper+0x7a>
    80004f1e:	00090763          	beqz	s2,80004f2c <fdt_scan_helper+0x7a>
    80004f22:	689c                	ld	a5,16(s1)
    80004f24:	c781                	beqz	a5,80004f2c <fdt_scan_helper+0x7a>
    80004f26:	708c                	ld	a1,32(s1)
    80004f28:	854a                	mv	a0,s2
    80004f2a:	9782                	jalr	a5
        return lex + 1;
    80004f2c:	00440513          	addi	a0,s0,4
        if (!last && node && cb->done) cb->done(node, cb->extra);
        return lex;
      }
    }
  }
}
    80004f30:	70a6                	ld	ra,104(sp)
    80004f32:	7406                	ld	s0,96(sp)
    80004f34:	64e6                	ld	s1,88(sp)
    80004f36:	6946                	ld	s2,80(sp)
    80004f38:	69a6                	ld	s3,72(sp)
    80004f3a:	6a06                	ld	s4,64(sp)
    80004f3c:	6165                	addi	sp,sp,112
    80004f3e:	8082                	ret
        lex = lex_next;
    80004f40:	8452                	mv	s0,s4
        last = 1;
    80004f42:	4a05                	li	s4,1
    80004f44:	bf49                	j	80004ed6 <fdt_scan_helper+0x24>
    switch (bswap(lex[0])) {
    80004f46:	4711                	li	a4,4
    80004f48:	18e79a63          	bne	a5,a4,800050dc <fdt_scan_helper+0x22a>
        lex += 1;
    80004f4c:	0411                	addi	s0,s0,4
        break;
    80004f4e:	b761                	j	80004ed6 <fdt_scan_helper+0x24>
        assert(!last);
    80004f50:	100a1063          	bnez	s4,80005050 <fdt_scan_helper+0x19e>
        prop.name = strings + bswap(lex[2]);
    80004f54:	4418                	lw	a4,8(s0)
  uint32 y = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    80004f56:	0087179b          	slliw	a5,a4,0x8
    80004f5a:	ff010637          	lui	a2,0xff010
    80004f5e:	f0060613          	addi	a2,a2,-256 # ffffffffff00ff00 <_end+0xffffffff7effef00>
    80004f62:	8ff1                	and	a5,a5,a2
    80004f64:	2781                	sext.w	a5,a5
    80004f66:	0087569b          	srliw	a3,a4,0x8
    80004f6a:	00ff0737          	lui	a4,0xff0
    80004f6e:	0ff70713          	addi	a4,a4,255 # ff00ff <elf_fpread-0x7f00ff01>
    80004f72:	8ef9                	and	a3,a3,a4
    80004f74:	8fd5                	or	a5,a5,a3
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
    80004f76:	0107951b          	slliw	a0,a5,0x10
    80004f7a:	0107d79b          	srliw	a5,a5,0x10
    80004f7e:	8d5d                	or	a0,a0,a5
        prop.name = strings + bswap(lex[2]);
    80004f80:	1502                	slli	a0,a0,0x20
    80004f82:	9101                	srli	a0,a0,0x20
    80004f84:	954e                	add	a0,a0,s3
    80004f86:	e82a                	sd	a0,16(sp)
        prop.len = bswap(lex[1]);
    80004f88:	4054                	lw	a3,4(s0)
  uint32 y = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    80004f8a:	0086979b          	slliw	a5,a3,0x8
    80004f8e:	8ff1                	and	a5,a5,a2
    80004f90:	2781                	sext.w	a5,a5
    80004f92:	0086d69b          	srliw	a3,a3,0x8
    80004f96:	8f75                	and	a4,a4,a3
    80004f98:	8fd9                	or	a5,a5,a4
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
    80004f9a:	0107971b          	slliw	a4,a5,0x10
    80004f9e:	0107d79b          	srliw	a5,a5,0x10
    80004fa2:	8fd9                	or	a5,a5,a4
        prop.len = bswap(lex[1]);
    80004fa4:	d03e                	sw	a5,32(sp)
        prop.value = lex + 3;
    80004fa6:	00c40793          	addi	a5,s0,12
    80004faa:	ec3e                	sd	a5,24(sp)
        if (node && !strcmp(prop.name, "#address-cells")) {
    80004fac:	08090163          	beqz	s2,8000502e <fdt_scan_helper+0x17c>
    80004fb0:	00004597          	auipc	a1,0x4
    80004fb4:	ef858593          	addi	a1,a1,-264 # 80008ea8 <rfs_i_ops+0x1e30>
    80004fb8:	9f2ff0ef          	jal	800041aa <strcmp>
    80004fbc:	e90d                	bnez	a0,80004fee <fdt_scan_helper+0x13c>
          node->address_cells = bswap(lex[3]);
    80004fbe:	4458                	lw	a4,12(s0)
  uint32 y = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    80004fc0:	0087179b          	slliw	a5,a4,0x8
    80004fc4:	ff0106b7          	lui	a3,0xff010
    80004fc8:	f0068693          	addi	a3,a3,-256 # ffffffffff00ff00 <_end+0xffffffff7effef00>
    80004fcc:	8ff5                	and	a5,a5,a3
    80004fce:	2781                	sext.w	a5,a5
    80004fd0:	0087569b          	srliw	a3,a4,0x8
    80004fd4:	00ff0737          	lui	a4,0xff0
    80004fd8:	0ff70713          	addi	a4,a4,255 # ff00ff <elf_fpread-0x7f00ff01>
    80004fdc:	8f75                	and	a4,a4,a3
    80004fde:	8fd9                	or	a5,a5,a4
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
    80004fe0:	0107971b          	slliw	a4,a5,0x10
    80004fe4:	0107d79b          	srliw	a5,a5,0x10
    80004fe8:	8fd9                	or	a5,a5,a4
          node->address_cells = bswap(lex[3]);
    80004fea:	00f92823          	sw	a5,16(s2)
        if (node && !strcmp(prop.name, "#size-cells")) {
    80004fee:	00004597          	auipc	a1,0x4
    80004ff2:	eca58593          	addi	a1,a1,-310 # 80008eb8 <rfs_i_ops+0x1e40>
    80004ff6:	6542                	ld	a0,16(sp)
    80004ff8:	9b2ff0ef          	jal	800041aa <strcmp>
    80004ffc:	e90d                	bnez	a0,8000502e <fdt_scan_helper+0x17c>
          node->size_cells = bswap(lex[3]);
    80004ffe:	4458                	lw	a4,12(s0)
  uint32 y = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    80005000:	0087179b          	slliw	a5,a4,0x8
    80005004:	ff0106b7          	lui	a3,0xff010
    80005008:	f0068693          	addi	a3,a3,-256 # ffffffffff00ff00 <_end+0xffffffff7effef00>
    8000500c:	8ff5                	and	a5,a5,a3
    8000500e:	2781                	sext.w	a5,a5
    80005010:	0087569b          	srliw	a3,a4,0x8
    80005014:	00ff0737          	lui	a4,0xff0
    80005018:	0ff70713          	addi	a4,a4,255 # ff00ff <elf_fpread-0x7f00ff01>
    8000501c:	8f75                	and	a4,a4,a3
    8000501e:	8fd9                	or	a5,a5,a4
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
    80005020:	0107971b          	slliw	a4,a5,0x10
    80005024:	0107d79b          	srliw	a5,a5,0x10
    80005028:	8fd9                	or	a5,a5,a4
          node->size_cells = bswap(lex[3]);
    8000502a:	00f92a23          	sw	a5,20(s2)
        lex += 3 + (prop.len + 3) / 4;
    8000502e:	5702                	lw	a4,32(sp)
    80005030:	270d                	addiw	a4,a4,3
    80005032:	41f7579b          	sraiw	a5,a4,0x1f
    80005036:	01e7d79b          	srliw	a5,a5,0x1e
    8000503a:	9fb9                	addw	a5,a5,a4
    8000503c:	4027d79b          	sraiw	a5,a5,0x2
    80005040:	278d                	addiw	a5,a5,3
    80005042:	078a                	slli	a5,a5,0x2
    80005044:	943e                	add	s0,s0,a5
        cb->prop(&prop, cb->extra);
    80005046:	649c                	ld	a5,8(s1)
    80005048:	708c                	ld	a1,32(s1)
    8000504a:	0028                	addi	a0,sp,8
    8000504c:	9782                	jalr	a5
        break;
    8000504e:	b561                	j	80004ed6 <fdt_scan_helper+0x24>
        assert(!last);
    80005050:	00004697          	auipc	a3,0x4
    80005054:	e3068693          	addi	a3,a3,-464 # 80008e80 <rfs_i_ops+0x1e08>
    80005058:	02400613          	li	a2,36
    8000505c:	00004597          	auipc	a1,0x4
    80005060:	e2c58593          	addi	a1,a1,-468 # 80008e88 <rfs_i_ops+0x1e10>
    80005064:	00002517          	auipc	a0,0x2
    80005068:	72450513          	addi	a0,a0,1828 # 80007788 <rfs_i_ops+0x710>
    8000506c:	b87ff0ef          	jal	80004bf2 <sprint>
    80005070:	6541                	lui	a0,0x10
    80005072:	157d                	addi	a0,a0,-1 # ffff <elf_fpread-0x7fff0001>
    80005074:	b9fff0ef          	jal	80004c12 <poweroff>
        if (!last && node && cb->done) cb->done(node, cb->extra);
    80005078:	000a1963          	bnez	s4,8000508a <fdt_scan_helper+0x1d8>
    8000507c:	00090763          	beqz	s2,8000508a <fdt_scan_helper+0x1d8>
    80005080:	689c                	ld	a5,16(s1)
    80005082:	c781                	beqz	a5,8000508a <fdt_scan_helper+0x1d8>
    80005084:	708c                	ld	a1,32(s1)
    80005086:	854a                	mv	a0,s2
    80005088:	9782                	jalr	a5
        child.name = (const char *)(lex + 1);
    8000508a:	00440793          	addi	a5,s0,4
    8000508e:	f83e                	sd	a5,48(sp)
        if (cb->open) cb->open(&child, cb->extra);
    80005090:	609c                	ld	a5,0(s1)
    80005092:	c781                	beqz	a5,8000509a <fdt_scan_helper+0x1e8>
    80005094:	708c                	ld	a1,32(s1)
    80005096:	1028                	addi	a0,sp,40
    80005098:	9782                	jalr	a5
        lex_next = fdt_scan_helper(lex + 2 + strlen(child.name) / 4, strings, &child, cb);
    8000509a:	7542                	ld	a0,48(sp)
    8000509c:	8fcff0ef          	jal	80004198 <strlen>
    800050a0:	9971                	andi	a0,a0,-4
    800050a2:	0521                	addi	a0,a0,8
    800050a4:	86a6                	mv	a3,s1
    800050a6:	1030                	addi	a2,sp,40
    800050a8:	85ce                	mv	a1,s3
    800050aa:	9522                	add	a0,a0,s0
    800050ac:	e07ff0ef          	jal	80004eb2 <fdt_scan_helper>
    800050b0:	8a2a                	mv	s4,a0
        if (cb->close && cb->close(&child, cb->extra) == -1)
    800050b2:	6c9c                	ld	a5,24(s1)
    800050b4:	e80786e3          	beqz	a5,80004f40 <fdt_scan_helper+0x8e>
    800050b8:	708c                	ld	a1,32(s1)
    800050ba:	1028                	addi	a0,sp,40
    800050bc:	9782                	jalr	a5
    800050be:	57fd                	li	a5,-1
    800050c0:	00f50963          	beq	a0,a5,800050d2 <fdt_scan_helper+0x220>
        lex = lex_next;
    800050c4:	8452                	mv	s0,s4
        last = 1;
    800050c6:	4a05                	li	s4,1
    800050c8:	b539                	j	80004ed6 <fdt_scan_helper+0x24>
          while (lex != lex_next) *lex++ = bswap(FDT_NOP);
    800050ca:	040007b7          	lui	a5,0x4000
    800050ce:	c01c                	sw	a5,0(s0)
    800050d0:	0411                	addi	s0,s0,4
    800050d2:	ff441ce3          	bne	s0,s4,800050ca <fdt_scan_helper+0x218>
        lex = lex_next;
    800050d6:	8452                	mv	s0,s4
        last = 1;
    800050d8:	4a05                	li	s4,1
    800050da:	bbf5                	j	80004ed6 <fdt_scan_helper+0x24>
        if (!last && node && cb->done) cb->done(node, cb->extra);
    800050dc:	000a1b63          	bnez	s4,800050f2 <fdt_scan_helper+0x240>
    800050e0:	00090b63          	beqz	s2,800050f6 <fdt_scan_helper+0x244>
    800050e4:	689c                	ld	a5,16(s1)
    800050e6:	cb91                	beqz	a5,800050fa <fdt_scan_helper+0x248>
    800050e8:	708c                	ld	a1,32(s1)
    800050ea:	854a                	mv	a0,s2
    800050ec:	9782                	jalr	a5
        return lex;
    800050ee:	8522                	mv	a0,s0
    800050f0:	b581                	j	80004f30 <fdt_scan_helper+0x7e>
    800050f2:	8522                	mv	a0,s0
    800050f4:	bd35                	j	80004f30 <fdt_scan_helper+0x7e>
    800050f6:	8522                	mv	a0,s0
    800050f8:	bd25                	j	80004f30 <fdt_scan_helper+0x7e>
    800050fa:	8522                	mv	a0,s0
    800050fc:	bd15                	j	80004f30 <fdt_scan_helper+0x7e>

00000000800050fe <fdt_get_address>:

const uint32 *fdt_get_address(const struct fdt_scan_node *node, const uint32 *value,
                              uint64 *result) {
  *result = 0;
    800050fe:	00063023          	sd	zero,0(a2)
  for (int cells = node->address_cells; cells > 0; --cells)
    80005102:	4908                	lw	a0,16(a0)
    80005104:	a091                	j	80005148 <fdt_get_address+0x4a>
    *result = (*result << 32) + bswap(*value++);
    80005106:	621c                	ld	a5,0(a2)
    80005108:	02079713          	slli	a4,a5,0x20
    8000510c:	4194                	lw	a3,0(a1)
  uint32 y = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    8000510e:	0086979b          	slliw	a5,a3,0x8
    80005112:	ff010837          	lui	a6,0xff010
    80005116:	f0080813          	addi	a6,a6,-256 # ffffffffff00ff00 <_end+0xffffffff7effef00>
    8000511a:	0107f7b3          	and	a5,a5,a6
    8000511e:	2781                	sext.w	a5,a5
    80005120:	0086d81b          	srliw	a6,a3,0x8
    80005124:	00ff06b7          	lui	a3,0xff0
    80005128:	0ff68693          	addi	a3,a3,255 # ff00ff <elf_fpread-0x7f00ff01>
    8000512c:	0106f6b3          	and	a3,a3,a6
    80005130:	8fd5                	or	a5,a5,a3
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
    80005132:	0107969b          	slliw	a3,a5,0x10
    80005136:	0107d79b          	srliw	a5,a5,0x10
    8000513a:	8fd5                	or	a5,a5,a3
    *result = (*result << 32) + bswap(*value++);
    8000513c:	1782                	slli	a5,a5,0x20
    8000513e:	9381                	srli	a5,a5,0x20
    80005140:	97ba                	add	a5,a5,a4
    80005142:	e21c                	sd	a5,0(a2)
  for (int cells = node->address_cells; cells > 0; --cells)
    80005144:	357d                	addiw	a0,a0,-1
    *result = (*result << 32) + bswap(*value++);
    80005146:	0591                	addi	a1,a1,4
  for (int cells = node->address_cells; cells > 0; --cells)
    80005148:	faa04fe3          	bgtz	a0,80005106 <fdt_get_address+0x8>
  return value;
}
    8000514c:	852e                	mv	a0,a1
    8000514e:	8082                	ret

0000000080005150 <fdt_get_size>:

const uint32 *fdt_get_size(const struct fdt_scan_node *node, const uint32 *value, uint64 *result) {
  *result = 0;
    80005150:	00063023          	sd	zero,0(a2)
  for (int cells = node->size_cells; cells > 0; --cells)
    80005154:	4948                	lw	a0,20(a0)
    80005156:	a091                	j	8000519a <fdt_get_size+0x4a>
    *result = (*result << 32) + bswap(*value++);
    80005158:	621c                	ld	a5,0(a2)
    8000515a:	02079713          	slli	a4,a5,0x20
    8000515e:	4194                	lw	a3,0(a1)
  uint32 y = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    80005160:	0086979b          	slliw	a5,a3,0x8
    80005164:	ff010837          	lui	a6,0xff010
    80005168:	f0080813          	addi	a6,a6,-256 # ffffffffff00ff00 <_end+0xffffffff7effef00>
    8000516c:	0107f7b3          	and	a5,a5,a6
    80005170:	2781                	sext.w	a5,a5
    80005172:	0086d81b          	srliw	a6,a3,0x8
    80005176:	00ff06b7          	lui	a3,0xff0
    8000517a:	0ff68693          	addi	a3,a3,255 # ff00ff <elf_fpread-0x7f00ff01>
    8000517e:	0106f6b3          	and	a3,a3,a6
    80005182:	8fd5                	or	a5,a5,a3
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
    80005184:	0107969b          	slliw	a3,a5,0x10
    80005188:	0107d79b          	srliw	a5,a5,0x10
    8000518c:	8fd5                	or	a5,a5,a3
    *result = (*result << 32) + bswap(*value++);
    8000518e:	1782                	slli	a5,a5,0x20
    80005190:	9381                	srli	a5,a5,0x20
    80005192:	97ba                	add	a5,a5,a4
    80005194:	e21c                	sd	a5,0(a2)
  for (int cells = node->size_cells; cells > 0; --cells)
    80005196:	357d                	addiw	a0,a0,-1
    *result = (*result << 32) + bswap(*value++);
    80005198:	0591                	addi	a1,a1,4
  for (int cells = node->size_cells; cells > 0; --cells)
    8000519a:	faa04fe3          	bgtz	a0,80005158 <fdt_get_size+0x8>
  return value;
}
    8000519e:	852e                	mv	a0,a1
    800051a0:	8082                	ret

00000000800051a2 <fdt_scan>:

void fdt_scan(uint64 fdt, const struct fdt_cb *cb) {
  struct fdt_header *header = (struct fdt_header *)fdt;

  // Only process FDT that we understand
  if (bswap(header->magic) != FDT_MAGIC || bswap(header->last_comp_version) > FDT_VERSION) return;
    800051a2:	411c                	lw	a5,0(a0)
  uint32 y = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    800051a4:	0087971b          	slliw	a4,a5,0x8
    800051a8:	ff010637          	lui	a2,0xff010
    800051ac:	f0060613          	addi	a2,a2,-256 # ffffffffff00ff00 <_end+0xffffffff7effef00>
    800051b0:	8f71                	and	a4,a4,a2
    800051b2:	2701                	sext.w	a4,a4
    800051b4:	0087d61b          	srliw	a2,a5,0x8
    800051b8:	00ff07b7          	lui	a5,0xff0
    800051bc:	0ff78793          	addi	a5,a5,255 # ff00ff <elf_fpread-0x7f00ff01>
    800051c0:	8ff1                	and	a5,a5,a2
    800051c2:	8fd9                	or	a5,a5,a4
  if (bswap(header->magic) != FDT_MAGIC || bswap(header->last_comp_version) > FDT_VERSION) return;
    800051c4:	feedd737          	lui	a4,0xfeedd
    800051c8:	0735                	addi	a4,a4,13 # fffffffffeedd00d <_end+0xffffffff7eecc00d>
    800051ca:	02e79d63          	bne	a5,a4,80005204 <fdt_scan+0x62>
    800051ce:	86ae                	mv	a3,a1
    800051d0:	4d18                	lw	a4,24(a0)
  uint32 y = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    800051d2:	0087179b          	slliw	a5,a4,0x8
    800051d6:	ff010637          	lui	a2,0xff010
    800051da:	f0060613          	addi	a2,a2,-256 # ffffffffff00ff00 <_end+0xffffffff7effef00>
    800051de:	8ff1                	and	a5,a5,a2
    800051e0:	2781                	sext.w	a5,a5
    800051e2:	0087561b          	srliw	a2,a4,0x8
    800051e6:	00ff0737          	lui	a4,0xff0
    800051ea:	0ff70713          	addi	a4,a4,255 # ff00ff <elf_fpread-0x7f00ff01>
    800051ee:	8f71                	and	a4,a4,a2
    800051f0:	8fd9                	or	a5,a5,a4
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
    800051f2:	0107971b          	slliw	a4,a5,0x10
    800051f6:	0107d79b          	srliw	a5,a5,0x10
    800051fa:	8fd9                	or	a5,a5,a4
    800051fc:	2781                	sext.w	a5,a5
  if (bswap(header->magic) != FDT_MAGIC || bswap(header->last_comp_version) > FDT_VERSION) return;
    800051fe:	4745                	li	a4,17
    80005200:	00f77363          	bgeu	a4,a5,80005206 <fdt_scan+0x64>
    80005204:	8082                	ret
void fdt_scan(uint64 fdt, const struct fdt_cb *cb) {
    80005206:	1141                	addi	sp,sp,-16
    80005208:	e406                	sd	ra,8(sp)

  const char *strings = (const char *)(fdt + bswap(header->off_dt_strings));
    8000520a:	4558                	lw	a4,12(a0)
  uint32 y = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    8000520c:	0087179b          	slliw	a5,a4,0x8
    80005210:	ff010837          	lui	a6,0xff010
    80005214:	f0080813          	addi	a6,a6,-256 # ffffffffff00ff00 <_end+0xffffffff7effef00>
    80005218:	0107f7b3          	and	a5,a5,a6
    8000521c:	2781                	sext.w	a5,a5
    8000521e:	0087561b          	srliw	a2,a4,0x8
    80005222:	00ff0737          	lui	a4,0xff0
    80005226:	0ff70713          	addi	a4,a4,255 # ff00ff <elf_fpread-0x7f00ff01>
    8000522a:	8e79                	and	a2,a2,a4
    8000522c:	8fd1                	or	a5,a5,a2
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
    8000522e:	0107959b          	slliw	a1,a5,0x10
    80005232:	0107d79b          	srliw	a5,a5,0x10
    80005236:	8ddd                	or	a1,a1,a5
  const char *strings = (const char *)(fdt + bswap(header->off_dt_strings));
    80005238:	1582                	slli	a1,a1,0x20
    8000523a:	9181                	srli	a1,a1,0x20
  uint32 *lex = (uint32 *)(fdt + bswap(header->off_dt_struct));
    8000523c:	4510                	lw	a2,8(a0)
  uint32 y = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    8000523e:	0086179b          	slliw	a5,a2,0x8
    80005242:	0107f7b3          	and	a5,a5,a6
    80005246:	2781                	sext.w	a5,a5
    80005248:	0086561b          	srliw	a2,a2,0x8
    8000524c:	8f71                	and	a4,a4,a2
    8000524e:	8fd9                	or	a5,a5,a4
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
    80005250:	0107971b          	slliw	a4,a5,0x10
    80005254:	0107d79b          	srliw	a5,a5,0x10
    80005258:	8fd9                	or	a5,a5,a4
  uint32 *lex = (uint32 *)(fdt + bswap(header->off_dt_struct));
    8000525a:	1782                	slli	a5,a5,0x20
    8000525c:	9381                	srli	a5,a5,0x20

  fdt_scan_helper(lex, strings, 0, cb);
    8000525e:	4601                	li	a2,0
    80005260:	95aa                	add	a1,a1,a0
    80005262:	953e                	add	a0,a0,a5
    80005264:	c4fff0ef          	jal	80004eb2 <fdt_scan_helper>
}
    80005268:	60a2                	ld	ra,8(sp)
    8000526a:	0141                	addi	sp,sp,16
    8000526c:	8082                	ret
	...

0000000080006000 <_trap_sec_start>:
    80006000:	14051573          	csrrw	a0,sscratch,a0
    80006004:	8faa                	mv	t6,a0
    80006006:	001fb023          	sd	ra,0(t6)
    8000600a:	002fb423          	sd	sp,8(t6)
    8000600e:	003fb823          	sd	gp,16(t6)
    80006012:	004fbc23          	sd	tp,24(t6)
    80006016:	025fb023          	sd	t0,32(t6)
    8000601a:	026fb423          	sd	t1,40(t6)
    8000601e:	027fb823          	sd	t2,48(t6)
    80006022:	028fbc23          	sd	s0,56(t6)
    80006026:	049fb023          	sd	s1,64(t6)
    8000602a:	04afb423          	sd	a0,72(t6)
    8000602e:	04bfb823          	sd	a1,80(t6)
    80006032:	04cfbc23          	sd	a2,88(t6)
    80006036:	06dfb023          	sd	a3,96(t6)
    8000603a:	06efb423          	sd	a4,104(t6)
    8000603e:	06ffb823          	sd	a5,112(t6)
    80006042:	070fbc23          	sd	a6,120(t6)
    80006046:	091fb023          	sd	a7,128(t6)
    8000604a:	092fb423          	sd	s2,136(t6)
    8000604e:	093fb823          	sd	s3,144(t6)
    80006052:	094fbc23          	sd	s4,152(t6)
    80006056:	0b5fb023          	sd	s5,160(t6)
    8000605a:	0b6fb423          	sd	s6,168(t6)
    8000605e:	0b7fb823          	sd	s7,176(t6)
    80006062:	0b8fbc23          	sd	s8,184(t6)
    80006066:	0d9fb023          	sd	s9,192(t6)
    8000606a:	0dafb423          	sd	s10,200(t6)
    8000606e:	0dbfb823          	sd	s11,208(t6)
    80006072:	0dcfbc23          	sd	t3,216(t6)
    80006076:	0fdfb023          	sd	t4,224(t6)
    8000607a:	0fefb423          	sd	t5,232(t6)
    8000607e:	0fffb823          	sd	t6,240(t6)
    80006082:	140022f3          	csrr	t0,sscratch
    80006086:	04553423          	sd	t0,72(a0)
    8000608a:	0f853103          	ld	sp,248(a0)
    8000608e:	10053283          	ld	t0,256(a0)
    80006092:	11053303          	ld	t1,272(a0)
    80006096:	18031073          	csrw	satp,t1
    8000609a:	12000073          	sfence.vma
    8000609e:	8282                	jr	t0

00000000800060a0 <return_to_user>:
    800060a0:	18059073          	csrw	satp,a1
    800060a4:	12000073          	sfence.vma
    800060a8:	14051073          	csrw	sscratch,a0
    800060ac:	8faa                	mv	t6,a0
    800060ae:	000fb083          	ld	ra,0(t6)
    800060b2:	008fb103          	ld	sp,8(t6)
    800060b6:	010fb183          	ld	gp,16(t6)
    800060ba:	018fb203          	ld	tp,24(t6)
    800060be:	020fb283          	ld	t0,32(t6)
    800060c2:	028fb303          	ld	t1,40(t6)
    800060c6:	030fb383          	ld	t2,48(t6)
    800060ca:	038fb403          	ld	s0,56(t6)
    800060ce:	040fb483          	ld	s1,64(t6)
    800060d2:	048fb503          	ld	a0,72(t6)
    800060d6:	050fb583          	ld	a1,80(t6)
    800060da:	058fb603          	ld	a2,88(t6)
    800060de:	060fb683          	ld	a3,96(t6)
    800060e2:	068fb703          	ld	a4,104(t6)
    800060e6:	070fb783          	ld	a5,112(t6)
    800060ea:	078fb803          	ld	a6,120(t6)
    800060ee:	080fb883          	ld	a7,128(t6)
    800060f2:	088fb903          	ld	s2,136(t6)
    800060f6:	090fb983          	ld	s3,144(t6)
    800060fa:	098fba03          	ld	s4,152(t6)
    800060fe:	0a0fba83          	ld	s5,160(t6)
    80006102:	0a8fbb03          	ld	s6,168(t6)
    80006106:	0b0fbb83          	ld	s7,176(t6)
    8000610a:	0b8fbc03          	ld	s8,184(t6)
    8000610e:	0c0fbc83          	ld	s9,192(t6)
    80006112:	0c8fbd03          	ld	s10,200(t6)
    80006116:	0d0fbd83          	ld	s11,208(t6)
    8000611a:	0d8fbe03          	ld	t3,216(t6)
    8000611e:	0e0fbe83          	ld	t4,224(t6)
    80006122:	0e8fbf03          	ld	t5,232(t6)
    80006126:	0f0fbf83          	ld	t6,240(t6)
    8000612a:	10200073          	sret
	...
