run surf_data
run orb_data
run orb_data2
run orb_data3
run orb_data4
run orb_data5

AICK_surf_30_inf 		= originalAICKsurf_0_AICK_mat_pos;
AICK_surf_30_inf_time 	= originalAICKsurf_0_AICK_avg_time;
bowAICK_surf_30_inf		= bowAICKsurf_bl_0_2_0_BowAICKv2_mat_pos;
bowAICK_surf_30_inf_time= bowAICKsurf_bl_0_2_0_BowAICKv2_avg_time;

AICK_surf_5_200  		= originalAICKsurf_1_AICK_mat_pos;
AICK_surf_5_200_time 	= originalAICKsurf_1_AICK_avg_time;
bowAICK_surf_5_200     	= bowAICKsurf_bl_0_25_1_BowAICKv2_mat_pos;
bowAICK_surf_5_200_time	= bowAICKsurf_bl_0_25_1_BowAICKv2_avg_time;


AICK_orb_30_inf			= originalAICKorb_0_AICK_mat_pos;
AICK_orb_30_inf_time	= originalAICKorb_0_AICK_avg_time;
bowAICK_orb_30_inf		= bowAICKorb_bl_300_0_BowAICKv2_mat_pos;
bowAICK_orb_30_inf_time = bowAICKorb_bl_300_0_BowAICKv2_avg_time;

AICK_orb_10_300			= originalAICKorb_1_AICK_mat_pos;
AICK_orb_10_300_time 	= originalAICKorb_1_AICK_avg_time;
bowAICK_orb_10_300		= bowAICKorb_bl_330_1_BowAICKv2_mat_pos;
bowAICK_orb_10_300_time = bowAICKorb_bl_330_1_BowAICKv2_avg_time;

disp('Running matlab script');
figure(1)
clf
hold on
part = 1:75;
axis([0 thresholds(part(end)) 0 1.01])
step = 1;

plot(thresholds(part),AICK_surf_30_inf(step,(part)),'g')
plot(thresholds(part),AICK_surf_5_200(step,(part)),'g-x')

plot(thresholds(part),bowAICK_surf_30_inf(step,(part)),'m')
plot(thresholds(part),bowAICK_surf_5_200(step,(part)),'m-x')

plot(thresholds(part),AICK_orb_30_inf(step,(part)),'r')
plot(thresholds(part),AICK_orb_10_300(step,(part)),'r-x')

plot(thresholds(part),bowAICK_orb_30_inf(step,(part)),'b')
plot(thresholds(part),bowAICK_orb_10_300(step,(part)),'b-x')

legend('AICK surf','AICK surf fast','bow AICK surf','bow AICK surf fast','AICK orb','AICK orb fast','bow AICK orb','bow AICK orb fast')

%figure(2)
%clf
%hold on
%plot(AICK_surf_30_inf(:,15),'g')
%plot(AICK_surf_5_200(:,15),'g-x')
%plot(bowAICK_surf_30_inf(:,15),'m')
%plot(bowAICK_surf_5_200(:,15),'m-x')
%plot(AICK_orb_30_inf(:,15),'r')
%plot(AICK_orb_10_300(:,15),'r-x')
%plot(bowAICK_orb_30_inf(:,15),'b')
%plot(bowAICK_orb_10_300(:,15),'b-x')
legend('AICK surf','AICK surf fast','bow AICK surf','bow AICK surf fast','AICK orb','AICK orb fast','bow AICK orb','bow AICK orb fast')

figure(3)
clf
hold on
plot(log2(AICK_surf_30_inf_time),'g')
plot(log2(AICK_surf_5_200_time),'g-x')

plot(log2(bowAICK_surf_30_inf_time),'m')
plot(log2(bowAICK_surf_5_200_time),'m-x')

plot(log2(AICK_orb_30_inf_time),'r')
plot(log2(AICK_orb_10_300_time),'r-x')

plot(log2(bowAICK_orb_30_inf_time),'b')
plot(log2(bowAICK_orb_10_300_time),'b-x')

legend('AICK surf','AICK surf fast','bow AICK surf','bow AICK surf fast','AICK orb','AICK orb fast','bow AICK orb','bow AICK orb fast')

figure(4)
clf
hold on
%plot(thresholds(part),originalAICKorbIterations_0_AICK_mat_pos(step,(part)),'r')
%plot(thresholds(part),originalAICKorbIterations_1_AICK_mat_pos(step,(part)),'g')
%plot(thresholds(part),originalAICKorbIterations_2_AICK_mat_pos(step,(part)),'b')
%plot(thresholds(part),originalAICKorbIterations_3_AICK_mat_pos(step,(part)),'m')
%plot(thresholds(part),originalAICKorbIterations_4_AICK_mat_pos(step,(part)),'c')

plot(thresholds(part),bowAICKorb_wordthreshold_bl_250_0_BowAICKv2_mat_pos(step,(part)),'r')
plot(thresholds(part),bowAICKorb_wordthreshold_bl_290_0_BowAICKv2_mat_pos(step,(part)),'g')
plot(thresholds(part),bowAICKorb_wordthreshold_bl_330_0_BowAICKv2_mat_pos(step,(part)),'b')
plot(thresholds(part),bowAICKorb_wordthreshold_bl_370_0_BowAICKv2_mat_pos(step,(part)),'m')
plot(thresholds(part),bowAICKorb_wordthreshold_bl_410_0_BowAICKv2_mat_pos(step,(part)),'c')
figure(5)
clf
hold on
plot(log2(bowAICKorb_wordthreshold_bl_250_0_BowAICKv2_avg_time),'r')
plot(log2(bowAICKorb_wordthreshold_bl_290_0_BowAICKv2_avg_time),'g')
plot(log2(bowAICKorb_wordthreshold_bl_330_0_BowAICKv2_avg_time),'b')
plot(log2(bowAICKorb_wordthreshold_bl_370_0_BowAICKv2_avg_time),'m')
plot(log2(bowAICKorb_wordthreshold_bl_410_0_BowAICKv2_avg_time),'c')

figure(6)
clf
hold on
plot(thresholds(part),bowAICKorb_100words_wordthreshold_bl_330_0_BowAICKv2_mat_pos(step,(part)),'r')
plot(thresholds(part),bowAICKorb_100words_wordthreshold_bl_350_0_BowAICKv2_mat_pos(step,(part)),'g')
plot(thresholds(part),bowAICKorb_100words_wordthreshold_bl_370_0_BowAICKv2_mat_pos(step,(part)),'b')
plot(thresholds(part),bowAICKorb_100words_wordthreshold_bl_390_0_BowAICKv2_mat_pos(step,(part)),'m')

plot(thresholds(part),bowAICKorb_500words_wordthreshold_bl_330_0_BowAICKv2_mat_pos(step,(part)),'r-.')
plot(thresholds(part),bowAICKorb_500words_wordthreshold_bl_350_0_BowAICKv2_mat_pos(step,(part)),'g-.')
plot(thresholds(part),bowAICKorb_500words_wordthreshold_bl_370_0_BowAICKv2_mat_pos(step,(part)),'b-.')

plot(thresholds(part),bowAICKorb_1000words_wordthreshold_bl_310_0_BowAICKv2_mat_pos(step,(part)),'r-x')
plot(thresholds(part),bowAICKorb_1000words_wordthreshold_bl_330_0_BowAICKv2_mat_pos(step,(part)),'g-x')
plot(thresholds(part),bowAICKorb_1000words_wordthreshold_bl_350_0_BowAICKv2_mat_pos(step,(part)),'b-x')

plot(thresholds(part),bowAICKorb_5000words_wordthreshold_bl_290_0_BowAICKv2_mat_pos(step,(part)),'r-o')
plot(thresholds(part),bowAICKorb_5000words_wordthreshold_bl_310_0_BowAICKv2_mat_pos(step,(part)),'g-o')
plot(thresholds(part),bowAICKorb_5000words_wordthreshold_bl_330_0_BowAICKv2_mat_pos(step,(part)),'b-o')

