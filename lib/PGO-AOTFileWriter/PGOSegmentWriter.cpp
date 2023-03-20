#include "PGO-AOTFileWriter/PGOSegmentWriter.hpp"
#include <cstdlib>
#include <climits>
#include <cstring>

void PGOSegmentsWriter::write_to_buf(char* buf, std::vector<Segment*>& segs, AOT_Header* hdr)
{
      u_int32_t seg_count = segs.size();
            
      AOT_Segment* cur_seg = (AOT_Segment* )buf;

      hdr->segtab_offset = (uintptr_t)cur_seg - (uintptr_t)hdr;
      hdr->segnum = seg_count;

      char* segments_name_begin = (char*)(cur_seg + seg_count);
      char* segments_name_end = (segments_name_begin + (seg_count * PATH_MAX));
      
      char* segment_name_current = segments_name_begin;
      
      u_int32_t tb_total = 0;
      u_int32_t rel_total = 0;

      for(auto seg_ptr : segs)
      {
            std::memcpy(segment_name_current,
                        (seg_ptr->lib_name).c_str(),
                        (seg_ptr->lib_name).length());
            cur_seg->lib_name_offset = (uintptr_t)segment_name_current - (uintptr_t)hdr;
            *(segment_name_current + (seg_ptr->lib_name).length()) = '\0';
            segment_name_current += PATH_MAX;
            
            tb_total += seg_ptr->how_many_tbs();
            rel_total += seg_ptr->how_many_rel();

            cur_seg += 1;
      }
      
      AOT_TB* tb_table_cur = (AOT_TB*) segments_name_end;
      AOT_rel* rel_cur = (AOT_rel*)(tb_table_cur + tb_total);
      char* code_cur = (char*)(rel_cur + rel_total);
      cur_seg = (AOT_Segment* )buf;
      
      for(auto seg_ptr : segs)
      {
           memcpy(&cur_seg->info, &seg_ptr->seg->info, sizeof(AOT_SegInfo));
           cur_seg->ismap = seg_ptr->seg->ismap;
           cur_seg->tb_offset = (uintptr_t)tb_table_cur - (uintptr_t)hdr;
           cur_seg->tb_nums = seg_ptr->_tbs.size();

           PGOTBWriter::write_to_buf(hdr, cur_seg, tb_table_cur, rel_cur, code_cur, seg_ptr->_tbs);

           cur_seg += 1;
           tb_table_cur += seg_ptr->how_many_tbs();
           rel_cur += seg_ptr->how_many_rel();
           code_cur += seg_ptr->how_many_code();
      }
}
