﻿




/*
将某整数的数据位右移指定位数，返回移动后的结果
<int> 类型变量 <欲移动的位数>

*/
KrnlnApi int 右移(int 欲移动的整数, int 欲移动的位数) {
	int n = 欲移动的整数 >> 欲移动的位数;

	return n;

}