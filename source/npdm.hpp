        struct NpdmHeader {
            u32 magic;
            u32 _0x4;
            u32 _0x8;
            u8 mmu_flags;
            u8 _0xD;
            u8 main_thread_prio;
            u8 default_cpuid;
            u32 _0x10;
            u32 system_resource_size;
            u32 process_category;
            u32 main_stack_size;
            char title_name[0x50];
            u32 aci0_offset;
            u32 aci0_size;
            u32 acid_offset;
            u32 acid_size;
        };
        struct NpdmAcid {
            u8 signature[0x100];
            u8 modulus[0x100];
            u32 magic;
            u32 size;
            u32 _0x208;
            u32 is_retail;
            u64 title_id_range_min;
            u64 title_id_range_max;
            u32 fac_offset;
            u32 fac_size;
            u32 sac_offset;
            u32 sac_size;
            u32 kac_offset;
            u32 kac_size;
            u64 padding;
        };
        struct NpdmAci0 {
            u32 magic;
            u8 _0x4[0xC];
            u64 title_id;
            u64 _0x18;
            u32 fah_offset;
            u32 fah_size;
            u32 sac_offset;
            u32 sac_size;
            u32 kac_offset;
            u32 kac_size;
            u64 padding;
        };
        struct NpdmInfo {
            NpdmHeader *header;
            NpdmAcid *acid;
            NpdmAci0 *aci0;
            void *acid_fac;
            void *acid_sac;
            void *acid_kac;
            void *aci0_fah;
            void *aci0_sac;
            void *aci0_kac;
            u64 title_id;
        };
        struct NpdmCache {
            NpdmInfo info;
            u8 buffer[0x8000];
        };



void patch_npdm(char* path, u64 new_tid);
int write_lfs(char* path, u64 new_tid);