api = 'http://vanderlei.ga/cervical/';
url = [api 'test.json'];
S = webread(url);
year = [S.year];
data = [S.data];
plot(year,data)
xlabel('Year');
ylabel('Temperature (Celsius)');
title('USA Average Temperatures')
axis tight
