Project BPMPS is MPS System for BP.

- Creator: Yue Li
- Init Date : 2010-10-30

如何构造 skucode_t
insert into skucode_t(skucode,jdewh) select jdeskucode,jdewh from openinv_o group by jdeskucode,jdewh;
delete from skucode_t where pkid>200;

insert into skucode_t(skucode,jdewh)
  select modelengine_o.jdeskucode,skucode_t_copy.jdewh from modelengine_o,skucode_t_copy
    where modelengine_o.jdeskucode=skucode_t_copy.skucode group by modelengine_o.fullindex,modelengine_o.jdeskucode;
