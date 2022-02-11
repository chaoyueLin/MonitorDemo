Android 9.0 以前：/system/lib/libart.so

Android 10.0 在：/system/apex/com.android.adbd/lib/libart.so

Android 11.0 在：/system/apex/com.android.art.capex 这个压缩文件里面

获取 GetContendedMonitor 和 GetLockOwnerThreadId 函数地址。

	nm -D libart.so | grep -E 'GetContendedMonitor|GetLockOwnerThreadId'
	# 或
	objdump -tT libart.so | grep -E 'GetContendedMonitor|GetLockOwnerThreadId'