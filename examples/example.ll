; ModuleID = 'llvm-link'
source_filename = "llvm-link"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct._pobj_pcache = type { %struct.pmemobjpool*, i64, i32 }
%struct.pmemobjpool = type opaque
%struct.__jmp_buf_tag = type { [8 x i64], i32, %struct.__sigset_t }
%struct.__sigset_t = type { [16 x i64] }

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str.1 = private unnamed_addr constant [23 x i8] c"/dev/shm/spp_test.pool\00", align 1
@.str.2 = private unnamed_addr constant [9 x i8] c"spp_test\00", align 1
@.str.3 = private unnamed_addr constant [13 x i8] c"pool != NULL\00", align 1
@.str.4 = private unnamed_addr constant [10 x i8] c"example.c\00", align 1
@__PRETTY_FUNCTION__.main = private unnamed_addr constant [11 x i8] c"int main()\00", align 1
@.str.5 = private unnamed_addr constant [21 x i8] c"!OID_IS_NULL(proot_)\00", align 1
@.str.6 = private unnamed_addr constant [25 x i8] c"root oid %ld %ld ptr %p\0A\00", align 1
@str = private unnamed_addr constant [46 x i8] c"Failed to allocate or snapshot a dummy object\00", align 1
@.str.8 = private unnamed_addr constant [9 x i8] c"pool %p\0A\00", align 1
@.str.9 = private unnamed_addr constant [12 x i8] c"ptrs %p %p\0A\00", align 1
@_pobj_cache_invalidate = external dso_local local_unnamed_addr global i32, align 4
@_pobj_cached_pool = external thread_local local_unnamed_addr global %struct._pobj_pcache, align 8

; Function Attrs: alwaysinline norecurse nounwind readnone uwtable willreturn
define dso_local i8* @__spp_cleantag(i8* %0) local_unnamed_addr #0 {
  %2 = ptrtoint i8* %0 to i64
  %3 = and i64 %2, 4294967295
  %4 = inttoptr i64 %3 to i8*
  ret i8* %4
}

; Function Attrs: norecurse nounwind readnone uwtable willreturn
define dso_local i32 @test_func(i32 %0) local_unnamed_addr #1 {
  %2 = add nsw i32 %0, 1
  ret i32 %2
}

; Function Attrs: nofree norecurse nounwind uwtable willreturn writeonly
define dso_local void @test_func_ptr(i64* nocapture %0, i64 %1, i64* nocapture %2) local_unnamed_addr #2 {
  store i64 %1, i64* %0, align 8, !tbaa !2
  %4 = add i64 %1, 1
  store i64 %4, i64* %2, align 8, !tbaa !2
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local i32 @main() local_unnamed_addr #3 {
  %1 = alloca [1 x %struct.__jmp_buf_tag], align 16
  %2 = call i32 @test_func(i32 5)
  %3 = call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 %2)
  %4 = call noalias dereferenceable_or_null(800) i8* @malloc(i64 800) #14
  %5 = bitcast i8* %4 to i64*
  %6 = call noalias dereferenceable_or_null(800) i8* @malloc(i64 800) #14
  %7 = bitcast i8* %6 to i64*
  br label %12

8:                                                ; preds = %12
  call void @free(i8* %4) #14
  call void @free(i8* %6) #14
  %9 = call i32 @unlink(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.1, i64 0, i64 0)) #14
  %10 = call %struct.pmemobjpool* @pmemobj_create(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.2, i64 0, i64 0), i64 33554432, i32 438) #14
  %11 = icmp eq %struct.pmemobjpool* %10, null
  br i1 %11, label %18, label %19

12:                                               ; preds = %0, %12
  %13 = phi i64 [ 0, %0 ], [ %16, %12 ]
  %14 = getelementptr inbounds i64, i64* %5, i64 %13
  %15 = getelementptr inbounds i64, i64* %7, i64 %13
  call void @test_func_ptr(i64* %14, i64 %13, i64* %15)
  %16 = add nuw nsw i64 %13, 1
  %17 = icmp eq i64 %16, 100
  br i1 %17, label %8, label %12, !llvm.loop !6

18:                                               ; preds = %8
  call void @__assert_fail(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.3, i64 0, i64 0), i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.4, i64 0, i64 0), i32 63, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @__PRETTY_FUNCTION__.main, i64 0, i64 0)) #15
  unreachable

19:                                               ; preds = %8
  %20 = call { i64, i64 } @pmemobj_root(%struct.pmemobjpool* nonnull %10, i64 16) #14
  %21 = extractvalue { i64, i64 } %20, 0
  %22 = extractvalue { i64, i64 } %20, 1
  %23 = icmp eq i64 %22, 0
  br i1 %23, label %24, label %25

24:                                               ; preds = %19
  call void @__assert_fail(i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str.5, i64 0, i64 0), i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.4, i64 0, i64 0), i32 66, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @__PRETTY_FUNCTION__.main, i64 0, i64 0)) #15
  unreachable

25:                                               ; preds = %19
  %26 = call fastcc i8* @pmemobj_direct_inline(i64 %21, i64 %22)
  %27 = call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([25 x i8], [25 x i8]* @.str.6, i64 0, i64 0), i64 %21, i64 %22, i8* %26)
  %28 = bitcast [1 x %struct.__jmp_buf_tag]* %1 to i8*
  call void @llvm.lifetime.start.p0i8(i64 200, i8* nonnull %28) #14
  %29 = getelementptr inbounds [1 x %struct.__jmp_buf_tag], [1 x %struct.__jmp_buf_tag]* %1, i64 0, i64 0
  %30 = call i32 @_setjmp(%struct.__jmp_buf_tag* nonnull %29) #16
  %31 = icmp eq i32 %30, 0
  br i1 %31, label %34, label %32

32:                                               ; preds = %25
  %33 = call i32 @pmemobj_tx_errno() #14
  br label %37

34:                                               ; preds = %25
  %35 = call i32 (%struct.pmemobjpool*, %struct.__jmp_buf_tag*, ...) @pmemobj_tx_begin(%struct.pmemobjpool* nonnull %10, %struct.__jmp_buf_tag* nonnull %29, i32 0, i32 0) #14
  %36 = icmp eq i32 %35, 0
  br i1 %36, label %40, label %37

37:                                               ; preds = %34, %32
  %38 = phi i32 [ %33, %32 ], [ %35, %34 ]
  %39 = call i32* @__errno_location() #17
  store i32 %38, i32* %39, align 4, !tbaa !9
  br label %40

40:                                               ; preds = %37, %34
  %41 = bitcast i8* %26 to i64*
  %42 = getelementptr inbounds i8, i8* %26, i64 8
  %43 = bitcast i8* %42 to i64*
  br label %44

44:                                               ; preds = %53, %40
  %45 = call i32 @pmemobj_tx_stage() #14
  switch i32 %45, label %53 [
    i32 0, label %54
    i32 1, label %46
    i32 3, label %51
  ]

46:                                               ; preds = %44
  %47 = call { i64, i64 } @pmemobj_tx_alloc(i64 16, i64 1) #14
  %48 = extractvalue { i64, i64 } %47, 0
  %49 = extractvalue { i64, i64 } %47, 1
  %50 = call i32 @pmemobj_tx_add_range_direct(i8* %26, i64 16) #14
  store i64 %48, i64* %41, align 8, !tbaa.struct !11
  store i64 %49, i64* %43, align 8, !tbaa.struct !12
  br label %53

51:                                               ; preds = %44
  %52 = call i32 @puts(i8* nonnull dereferenceable(1) getelementptr inbounds ([46 x i8], [46 x i8]* @str, i64 0, i64 0))
  call void @abort() #15
  unreachable

53:                                               ; preds = %44, %46
  call void @pmemobj_tx_process() #14
  br label %44, !llvm.loop !13

54:                                               ; preds = %44
  %55 = call i32 @pmemobj_tx_end() #14
  %56 = icmp eq i32 %55, 0
  br i1 %56, label %59, label %57

57:                                               ; preds = %54
  %58 = call i32* @__errno_location() #17
  store i32 %55, i32* %58, align 4, !tbaa !9
  br label %59

59:                                               ; preds = %57, %54
  call void @llvm.lifetime.end.p0i8(i64 200, i8* nonnull %28) #14
  %60 = call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([9 x i8], [9 x i8]* @.str.8, i64 0, i64 0), %struct.pmemobjpool* nonnull %10)
  %61 = bitcast i8* %26 to i64*
  %62 = load i64, i64* %61, align 8
  %63 = getelementptr inbounds i8, i8* %26, i64 8
  %64 = bitcast i8* %63 to i64*
  %65 = load i64, i64* %64, align 8
  %66 = call fastcc i8* @pmemobj_direct_inline(i64 %62, i64 %65)
  %67 = load i64, i64* %61, align 8
  %68 = load i64, i64* %64, align 8
  %69 = call fastcc i8* @pmemobj_direct_inline(i64 %67, i64 %68)
  %70 = getelementptr inbounds i8, i8* %69, i64 8
  %71 = call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([12 x i8], [12 x i8]* @.str.9, i64 0, i64 0), i8* %66, i8* nonnull %70)
  %72 = load i64, i64* %61, align 8
  %73 = load i64, i64* %64, align 8
  %74 = call fastcc i8* @pmemobj_direct_inline(i64 %72, i64 %73)
  %75 = bitcast i8* %74 to i64*
  store i64 1, i64* %75, align 8, !tbaa !2
  %76 = load i64, i64* %61, align 8
  %77 = load i64, i64* %64, align 8
  %78 = call fastcc i8* @pmemobj_direct_inline(i64 %76, i64 %77)
  %79 = getelementptr inbounds i8, i8* %78, i64 8
  %80 = bitcast i8* %79 to i64*
  store i64 2, i64* %80, align 8, !tbaa !2
  call void @pmemobj_close(%struct.pmemobjpool* nonnull %10) #14
  ret i32 0
}

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @printf(i8* nocapture noundef readonly, ...) local_unnamed_addr #4

; Function Attrs: inaccessiblememonly nofree nounwind willreturn
declare dso_local noalias noundef i8* @malloc(i64) local_unnamed_addr #5

; Function Attrs: inaccessiblemem_or_argmemonly nounwind willreturn
declare dso_local void @free(i8* nocapture noundef) local_unnamed_addr #6

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @unlink(i8* nocapture noundef readonly) local_unnamed_addr #4

declare dso_local %struct.pmemobjpool* @pmemobj_create(i8*, i8*, i64, i32) local_unnamed_addr #7

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) local_unnamed_addr #8

declare dso_local { i64, i64 } @pmemobj_root(%struct.pmemobjpool*, i64) local_unnamed_addr #7

; Function Attrs: inlinehint nounwind uwtable
define internal fastcc i8* @pmemobj_direct_inline(i64 %0, i64 %1) unnamed_addr #9 {
  %3 = icmp eq i64 %1, 0
  %4 = icmp eq i64 %0, 0
  %5 = or i1 %4, %3
  br i1 %5, label %23, label %6

6:                                                ; preds = %2
  %7 = load i32, i32* @_pobj_cache_invalidate, align 4, !tbaa !9
  %8 = load i32, i32* getelementptr inbounds (%struct._pobj_pcache, %struct._pobj_pcache* @_pobj_cached_pool, i64 0, i32 2), align 8, !tbaa !14
  %9 = icmp eq i32 %7, %8
  %10 = load i64, i64* getelementptr inbounds (%struct._pobj_pcache, %struct._pobj_pcache* @_pobj_cached_pool, i64 0, i32 1), align 8
  %11 = icmp eq i64 %10, %0
  %12 = and i1 %9, %11
  br i1 %12, label %18, label %13

13:                                               ; preds = %6
  store i32 %7, i32* getelementptr inbounds (%struct._pobj_pcache, %struct._pobj_pcache* @_pobj_cached_pool, i64 0, i32 2), align 8, !tbaa !14
  %14 = call %struct.pmemobjpool* @pmemobj_pool_by_oid(i64 %0, i64 %1) #14
  store %struct.pmemobjpool* %14, %struct.pmemobjpool** getelementptr inbounds (%struct._pobj_pcache, %struct._pobj_pcache* @_pobj_cached_pool, i64 0, i32 0), align 8, !tbaa !17
  %15 = icmp eq %struct.pmemobjpool* %14, null
  br i1 %15, label %16, label %17

16:                                               ; preds = %13
  store i64 0, i64* getelementptr inbounds (%struct._pobj_pcache, %struct._pobj_pcache* @_pobj_cached_pool, i64 0, i32 1), align 8, !tbaa !18
  br label %23

17:                                               ; preds = %13
  store i64 %0, i64* getelementptr inbounds (%struct._pobj_pcache, %struct._pobj_pcache* @_pobj_cached_pool, i64 0, i32 1), align 8, !tbaa !18
  br label %18

18:                                               ; preds = %6, %17
  %19 = load %struct.pmemobjpool*, %struct.pmemobjpool** getelementptr inbounds (%struct._pobj_pcache, %struct._pobj_pcache* @_pobj_cached_pool, i64 0, i32 0), align 8, !tbaa !17
  %20 = ptrtoint %struct.pmemobjpool* %19 to i64
  %21 = add i64 %20, %1
  %22 = inttoptr i64 %21 to i8*
  br label %23

23:                                               ; preds = %16, %18, %2
  %24 = phi i8* [ null, %2 ], [ %22, %18 ], [ null, %16 ]
  ret i8* %24
}

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #10

; Function Attrs: nounwind returns_twice
declare dso_local i32 @_setjmp(%struct.__jmp_buf_tag*) local_unnamed_addr #11

declare dso_local i32 @pmemobj_tx_errno() local_unnamed_addr #7

declare dso_local i32 @pmemobj_tx_begin(%struct.pmemobjpool*, %struct.__jmp_buf_tag*, ...) local_unnamed_addr #7

; Function Attrs: nounwind readnone willreturn
declare dso_local i32* @__errno_location() local_unnamed_addr #12

declare dso_local i32 @pmemobj_tx_stage() local_unnamed_addr #7

declare dso_local { i64, i64 } @pmemobj_tx_alloc(i64, i64) local_unnamed_addr #7

declare dso_local i32 @pmemobj_tx_add_range_direct(i8*, i64) local_unnamed_addr #7

; Function Attrs: nofree nounwind
declare noundef i32 @puts(i8* nocapture noundef readonly) local_unnamed_addr #13

; Function Attrs: noreturn nounwind
declare dso_local void @abort() local_unnamed_addr #8

declare dso_local void @pmemobj_tx_process() local_unnamed_addr #7

declare dso_local i32 @pmemobj_tx_end() local_unnamed_addr #7

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #10

declare dso_local void @pmemobj_close(%struct.pmemobjpool*) local_unnamed_addr #7

declare dso_local %struct.pmemobjpool* @pmemobj_pool_by_oid(i64, i64) local_unnamed_addr #7

attributes #0 = { alwaysinline norecurse nounwind readnone uwtable willreturn "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { norecurse nounwind readnone uwtable willreturn "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nofree norecurse nounwind uwtable willreturn writeonly "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind uwtable "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nofree nounwind "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { inaccessiblememonly nofree nounwind willreturn "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { inaccessiblemem_or_argmemonly nounwind willreturn "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #8 = { noreturn nounwind "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #9 = { inlinehint nounwind uwtable "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #10 = { argmemonly nofree nosync nounwind willreturn }
attributes #11 = { nounwind returns_twice "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #12 = { nounwind readnone willreturn "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #13 = { nofree nounwind }
attributes #14 = { nounwind }
attributes #15 = { noreturn nounwind }
attributes #16 = { nounwind returns_twice }
attributes #17 = { nounwind readnone willreturn }

!llvm.ident = !{!0, !0}
!llvm.module.flags = !{!1}

!0 = !{!"Ubuntu clang version 12.0.0-3ubuntu1~20.04.4"}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{!3, !3, i64 0}
!3 = !{!"long", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = distinct !{!6, !7, !8}
!7 = !{!"llvm.loop.mustprogress"}
!8 = !{!"llvm.loop.unroll.disable"}
!9 = !{!10, !10, i64 0}
!10 = !{!"int", !4, i64 0}
!11 = !{i64 0, i64 8, !2, i64 8, i64 8, !2}
!12 = !{i64 0, i64 8, !2}
!13 = distinct !{!13, !7, !8}
!14 = !{!15, !10, i64 16}
!15 = !{!"_pobj_pcache", !16, i64 0, !3, i64 8, !10, i64 16}
!16 = !{!"any pointer", !4, i64 0}
!17 = !{!15, !16, i64 0}
!18 = !{!15, !3, i64 8}
