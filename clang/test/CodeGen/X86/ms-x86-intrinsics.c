// RUN: %clang_cc1 -Werror -ffreestanding -fms-extensions -fms-compatibility -fms-compatibility-version=17.00 \
// RUN:         -triple i686--windows -Oz -emit-llvm %s -o - \
// RUN:         | FileCheck %s -check-prefix=CHECK -check-prefix=CHECK-I386
// RUN: %clang_cc1 -Werror -ffreestanding -fms-extensions -fms-compatibility -fms-compatibility-version=17.00 \
// RUN:         -triple x86_64--windows -Oz -emit-llvm %s -o - \
// RUN:         | FileCheck %s --check-prefix=CHECK --check-prefix=CHECK-X64

#include <intrin.h>

#if defined(__i386__)
char test__readfsbyte(unsigned long Offset) {
  return __readfsbyte(++Offset);
}
// CHECK-I386-LABEL: define dso_local signext i8 @test__readfsbyte(i32 noundef %Offset)
// CHECK-I386:   %inc = add i32 %Offset, 1
// CHECK-I386:   [[PTR:%[0-9]+]] = inttoptr i32 %inc to ptr addrspace(257)
// CHECK-I386:   [[VALUE:%[0-9]+]] = load volatile i8, ptr addrspace(257) [[PTR]], align 1
// CHECK-I386:   ret i8 [[VALUE:%[0-9]+]]

short test__readfsword(unsigned long Offset) {
  return __readfsword(++Offset);
}
// CHECK-I386-LABEL: define dso_local signext i16 @test__readfsword(i32 noundef %Offset)
// CHECK-I386:   %inc = add i32 %Offset, 1
// CHECK-I386:   [[PTR:%[0-9]+]] = inttoptr i32 %inc to ptr addrspace(257)
// CHECK-I386:   [[VALUE:%[0-9]+]] = load volatile i16, ptr addrspace(257) [[PTR]], align 2
// CHECK-I386:   ret i16 [[VALUE:%[0-9]+]]

long test__readfsdword(unsigned long Offset) {
  return __readfsdword(++Offset);
}
// CHECK-I386-LABEL: define dso_local i32 @test__readfsdword(i32 noundef %Offset)
// CHECK-I386:   %inc = add i32 %Offset, 1
// CHECK-I386:   [[PTR:%[0-9]+]] = inttoptr i32 %inc to ptr addrspace(257)
// CHECK-I386:   [[VALUE:%[0-9]+]] = load volatile i32, ptr addrspace(257) [[PTR]], align 4
// CHECK-I386:   ret i32 [[VALUE:%[0-9]+]]

long long test__readfsqword(unsigned long Offset) {
  return __readfsqword(++Offset);
}
// CHECK-I386-LABEL: define dso_local i64 @test__readfsqword(i32 noundef %Offset)
// CHECK-I386:   %inc = add i32 %Offset, 1
// CHECK-I386:   [[PTR:%[0-9]+]] = inttoptr i32 %inc to ptr addrspace(257)
// CHECK-I386:   [[VALUE:%[0-9]+]] = load volatile i64, ptr addrspace(257) [[PTR]], align 8
// CHECK-I386:   ret i64 [[VALUE:%[0-9]+]]
#endif

__int64 test__emul(int a, int b) {
  return __emul(a, b);
}
// CHECK-LABEL: define dso_local range(i64 -4611686016279904256, 4611686018427387905) i64 @test__emul(i32 noundef %a, i32 noundef %b)
// CHECK: [[X:%[0-9]+]] = sext i32 %a to i64
// CHECK: [[Y:%[0-9]+]] = sext i32 %b to i64
// CHECK: [[RES:%[0-9]+]] = mul nsw i64 [[Y]], [[X]]
// CHECK: ret i64 [[RES]]

unsigned __int64 test__emulu(unsigned int a, unsigned int b) {
  return __emulu(a, b);
}
// CHECK-LABEL: define dso_local range(i64 0, -8589934590) i64 @test__emulu(i32 noundef %a, i32 noundef %b)
// CHECK: [[X:%[0-9]+]] = zext i32 %a to i64
// CHECK: [[Y:%[0-9]+]] = zext i32 %b to i64
// CHECK: [[RES:%[0-9]+]] = mul nuw i64 [[Y]], [[X]]
// CHECK: ret i64 [[RES]]


unsigned char test_inbyte(unsigned short port) {
  return __inbyte(port);
}
// CHECK-LABEL: i8 @test_inbyte(i16 noundef
// CHECK-SAME:  [[PORT:%.*]])
// CHECK:       [[TMP0:%.*]] = tail call i8 asm sideeffect "inb ${1:w}, ${0:b}", "={ax},N{dx},~{dirflag},~{fpsr},~{flags}"(i16 [[PORT]])
// CHECK-NEXT:  ret i8 [[TMP0]]

unsigned short test_inword(unsigned short port) {
  return __inword(port);
}
// CHECK-LABEL: i16 @test_inword(i16 noundef
// CHECK-SAME:  [[PORT:%.*]])
// CHECK:       [[TMP0:%.*]] = tail call i16 asm sideeffect "inw ${1:w}, ${0:w}", "={ax},N{dx},~{dirflag},~{fpsr},~{flags}"(i16 [[PORT]])
// CHECK-NEXT:  ret i16 [[TMP0]]

unsigned long test_indword(unsigned short port) {
  return __indword(port);
}
// CHECK-LABEL: i32 @test_indword(i16 noundef
// CHECK-SAME:  [[PORT:%.*]])
// CHECK:       [[TMP0:%.*]] = tail call i32 asm sideeffect "inl ${1:w}, ${0:k}", "={ax},N{dx},~{dirflag},~{fpsr},~{flags}"(i16 [[PORT]])
// CHECK-NEXT:  ret i32 [[TMP0]]

void test_outbyte(unsigned short port, unsigned char data) {
    return __outbyte(port, data);
}
// CHECK-LABEL: void @test_outbyte(
// CHECK-SAME:  [[PORT:%.*]],
// CHECK-SAME:  [[DATA:%.*]])
// CHECK:       tail call void asm sideeffect "outb ${0:b}, ${1:w}", "{ax},N{dx},~{dirflag},~{fpsr},~{flags}"(i8 [[DATA]], i16 [[PORT]])

void test_outword(unsigned short port, unsigned short data) {
    return __outword(port, data);
}
// CHECK-LABEL: void @test_outword(
// CHECK-SAME:  [[PORT:%.*]],
// CHECK-SAME:  [[DATA:%.*]])
// CHECK:       tail call void asm sideeffect "outw ${0:w}, ${1:w}", "{ax},N{dx},~{dirflag},~{fpsr},~{flags}"(i16 [[DATA]], i16 [[PORT]])

void test_outdword(unsigned short port, unsigned long data) {
    return __outdword(port, data);
}
// CHECK-LABEL: void @test_outdword(
// CHECK-SAME:  [[PORT:%.*]],
// CHECK-SAME:  [[DATA:%.*]])
// CHECK:       tail call void asm sideeffect "outl ${0:k}, ${1:w}", "{ax},N{dx},~{dirflag},~{fpsr},~{flags}"(i32 [[DATA]], i16 [[PORT]])

#if defined(__x86_64__)

char test__readgsbyte(unsigned long Offset) {
  return __readgsbyte(++Offset);
}
// CHECK-X64-LABEL: define dso_local i8 @test__readgsbyte(i32 noundef %Offset)
// CHECK-X64:   %inc = add i32 %Offset, 1
// CHECK-X64:   [[ZEXT:%[0-9]+]] = zext i32 %inc to i64
// CHECK-X64:   [[PTR:%[0-9]+]] = inttoptr i64 [[ZEXT]] to ptr addrspace(256)
// CHECK-X64:   [[VALUE:%[0-9]+]] = load volatile i8, ptr addrspace(256) [[PTR]], align 1
// CHECK-X64:   ret i8 [[VALUE:%[0-9]+]]

short test__readgsword(unsigned long Offset) {
  return __readgsword(++Offset);
}
// CHECK-X64-LABEL: define dso_local i16 @test__readgsword(i32 noundef %Offset)
// CHECK-X64:   %inc = add i32 %Offset, 1
// CHECK-X64:   [[ZEXT:%[0-9]+]] = zext i32 %inc to i64
// CHECK-X64:   [[PTR:%[0-9]+]] = inttoptr i64 [[ZEXT]] to ptr addrspace(256)
// CHECK-X64:   [[VALUE:%[0-9]+]] = load volatile i16, ptr addrspace(256) [[PTR]], align 2
// CHECK-X64:   ret i16 [[VALUE:%[0-9]+]]

long test__readgsdword(unsigned long Offset) {
  return __readgsdword(++Offset);
}
// CHECK-X64-LABEL: define dso_local i32 @test__readgsdword(i32 noundef %Offset)
// CHECK-X64:   %inc = add i32 %Offset, 1
// CHECK-X64:   [[ZEXT:%[0-9]+]] = zext i32 %inc to i64
// CHECK-X64:   [[PTR:%[0-9]+]] = inttoptr i64 [[ZEXT]] to ptr addrspace(256)
// CHECK-X64:   [[VALUE:%[0-9]+]] = load volatile i32, ptr addrspace(256) [[PTR]], align 4
// CHECK-X64:   ret i32 [[VALUE:%[0-9]+]]

long long test__readgsqword(unsigned long Offset) {
  return __readgsqword(++Offset);
}
// CHECK-X64-LABEL: define dso_local i64 @test__readgsqword(i32 noundef %Offset)
// CHECK-X64:   %inc = add i32 %Offset, 1
// CHECK-X64:   [[ZEXT:%[0-9]+]] = zext i32 %inc to i64
// CHECK-X64:   [[PTR:%[0-9]+]] = inttoptr i64 [[ZEXT]] to ptr addrspace(256)
// CHECK-X64:   [[VALUE:%[0-9]+]] = load volatile i64, ptr addrspace(256) [[PTR]], align 8
// CHECK-X64:   ret i64 [[VALUE:%[0-9]+]]

__int64 test__mulh(__int64 a, __int64 b) {
  return __mulh(a, b);
}
// CHECK-X64-LABEL: define dso_local range(i64 -4611686018427387904, 4611686018427387905) i64 @test__mulh(i64 noundef %a, i64 noundef %b)
// CHECK-X64: = mul nsw i128 %

unsigned __int64 test__umulh(unsigned __int64 a, unsigned __int64 b) {
  return __umulh(a, b);
}
// CHECK-X64-LABEL: define dso_local range(i64 0, -1) i64 @test__umulh(i64 noundef %a, i64 noundef %b)
// CHECK-X64: = mul nuw i128 %

__int64 test_mul128(__int64 Multiplier,
                    __int64 Multiplicand,
                    __int64 *HighProduct) {
  return _mul128(Multiplier, Multiplicand, HighProduct);
}
// CHECK-X64-LABEL: define dso_local i64 @test_mul128(i64 noundef %Multiplier, i64 noundef %Multiplicand, ptr{{.*}} initializes((0, 8)) %HighProduct)
// CHECK-X64: = sext i64 %Multiplier to i128
// CHECK-X64: = sext i64 %Multiplicand to i128
// CHECK-X64: = mul nsw i128 %
// CHECK-X64: store i64 %
// CHECK-X64: ret i64 %

unsigned __int64 test_umul128(unsigned __int64 Multiplier,
                              unsigned __int64 Multiplicand,
                              unsigned __int64 *HighProduct) {
  return _umul128(Multiplier, Multiplicand, HighProduct);
}
// CHECK-X64-LABEL: define dso_local i64 @test_umul128(i64 noundef %Multiplier, i64 noundef %Multiplicand, ptr{{.*}} initializes((0, 8)) %HighProduct)
// CHECK-X64: = zext i64 %Multiplier to i128
// CHECK-X64: = zext i64 %Multiplicand to i128
// CHECK-X64: = mul nuw i128 %
// CHECK-X64: store i64 %
// CHECK-X64: ret i64 %

unsigned __int64 test__shiftleft128(unsigned __int64 l, unsigned __int64 h,
                                    unsigned char d) {
  return __shiftleft128(l, h, d);
}
// CHECK-X64-LABEL: define dso_local noundef i64 @test__shiftleft128(i64 noundef %l, i64 noundef %h, i8 noundef %d)
// CHECK-X64: = zext i8 %{{.*}} to i64
// CHECK-X64: = tail call i64 @llvm.fshl.i64(i64 %h, i64 %l, i64 %{{.*}})
// CHECK-X64:  ret i64 %

unsigned __int64 test__shiftright128(unsigned __int64 l, unsigned __int64 h,
                                     unsigned char d) {
  return __shiftright128(l, h, d);
}
// CHECK-X64-LABEL: define dso_local noundef i64 @test__shiftright128(i64 noundef %l, i64 noundef %h, i8 noundef %d)
// CHECK-X64: = zext i8 %{{.*}} to i64
// CHECK-X64: = tail call i64 @llvm.fshr.i64(i64 %h, i64 %l, i64 %{{.*}})
// CHECK-X64:  ret i64 %

signed char test_add_sat_i8(signed char x, signed char y) {
  return _sat_add_i8(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i8 @test_add_sat_i8(i8 noundef %x, i8 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i8 @llvm.sadd.sat.i8(i8 %x, i8 %y)
// CHECK-X64: ret i8 %[[VAR0:.*]]

signed short test_add_sat_i16(signed short x, signed short y) {
  return _sat_add_i16(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i16 @test_add_sat_i16(i16 noundef %x, i16 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i16 @llvm.sadd.sat.i16(i16 %x, i16 %y)
// CHECK-X64: ret i16 %[[VAR0:.*]]

signed int test_add_sat_i32(signed int x, signed int y) {
  return _sat_add_i32(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i32 @test_add_sat_i32(i32 noundef %x, i32 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i32 @llvm.sadd.sat.i32(i32 %x, i32 %y)
// CHECK-X64: ret i32 %[[VAR0:.*]]

signed __int64 test_add_sat_i64(signed __int64 x, signed __int64 y) {
  return _sat_add_i64(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i64 @test_add_sat_i64(i64 noundef %x, i64 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i64 @llvm.sadd.sat.i64(i64 %x, i64 %y)
// CHECK-X64: ret i64 %[[VAR0:.*]]

unsigned char test_add_sat_u8(unsigned char x, unsigned char y) {
  return _sat_add_u8(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i8 @test_add_sat_u8(i8 noundef %x, i8 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i8 @llvm.uadd.sat.i8(i8 %x, i8 %y)
// CHECK-X64: ret i8 %[[VAR0:.*]]

unsigned short test_add_sat_u16(unsigned short x, unsigned short y) {
  return _sat_add_u16(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i16 @test_add_sat_u16(i16 noundef %x, i16 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i16 @llvm.uadd.sat.i16(i16 %x, i16 %y)
// CHECK-X64: ret i16 %[[VAR0:.*]]

unsigned int test_add_sat_u32(unsigned int x, unsigned int y) {
  return _sat_add_u32(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i32 @test_add_sat_u32(i32 noundef %x, i32 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i32 @llvm.uadd.sat.i32(i32 %x, i32 %y)
// CHECK-X64: ret i32 %[[VAR0:.*]]

unsigned __int64 test_add_sat_u64(unsigned __int64 x, unsigned __int64 y) {
  return _sat_add_u64(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i64 @test_add_sat_u64(i64 noundef %x, i64 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i64 @llvm.uadd.sat.i64(i64 %x, i64 %y)
// CHECK-X64: ret i64 %[[VAR0:.*]]

signed char test_sub_sat_i8(signed char x, signed char y) {
  return _sat_sub_i8(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i8 @test_sub_sat_i8(i8 noundef %x, i8 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i8 @llvm.ssub.sat.i8(i8 %x, i8 %y)
// CHECK-X64: ret i8 %[[VAR0:.*]]

signed short test_sub_sat_i16(signed short x, signed short y) {
  return _sat_sub_i16(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i16 @test_sub_sat_i16(i16 noundef %x, i16 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i16 @llvm.ssub.sat.i16(i16 %x, i16 %y)
// CHECK-X64: ret i16 %[[VAR0:.*]]

signed int test_sub_sat_i32(signed int x, signed int y) {
  return _sat_sub_i32(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i32 @test_sub_sat_i32(i32 noundef %x, i32 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i32 @llvm.ssub.sat.i32(i32 %x, i32 %y)
// CHECK-X64: ret i32 %[[VAR0:.*]]

signed __int64 test_sub_sat_i64(signed __int64 x, signed __int64 y) {
  return _sat_sub_i64(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i64 @test_sub_sat_i64(i64 noundef %x, i64 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i64 @llvm.ssub.sat.i64(i64 %x, i64 %y)
// CHECK-X64: ret i64 %[[VAR0:.*]]

unsigned char test_sub_sat_u8(unsigned char x, unsigned char y) {
  return _sat_sub_u8(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i8 @test_sub_sat_u8(i8 noundef %x, i8 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i8 @llvm.usub.sat.i8(i8 %x, i8 %y)
// CHECK-X64: ret i8 %[[VAR0:.*]]

unsigned short test_sub_sat_u16(unsigned short x, unsigned short y) {
  return _sat_sub_u16(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i16 @test_sub_sat_u16(i16 noundef %x, i16 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i16 @llvm.usub.sat.i16(i16 %x, i16 %y)
// CHECK-X64: ret i16 %[[VAR0:.*]]

unsigned int test_sub_sat_u32(unsigned int x, unsigned int y) {
  return _sat_sub_u32(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i32 @test_sub_sat_u32(i32 noundef %x, i32 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i32 @llvm.usub.sat.i32(i32 %x, i32 %y)
// CHECK-X64: ret i32 %[[VAR0:.*]]

unsigned __int64 test_sub_sat_u64(unsigned __int64 x, unsigned __int64 y) {
  return _sat_sub_u64(x, y);
}
// CHECK-X64-LABEL: define dso_local noundef i64 @test_sub_sat_u64(i64 noundef %x, i64 noundef %y)
// CHECK-X64: %[[VAR0:.*]] = tail call i64 @llvm.usub.sat.i64(i64 %x, i64 %y)
// CHECK-X64: ret i64 %[[VAR0:.*]]

#endif // defined(__x86_64__)
