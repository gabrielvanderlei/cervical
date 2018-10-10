api = 'http://vanderlei.ga/cervical/';
url = [api 'test.json'];
S = webread(url);
X = [S.X];
Y = [S.Y];
plot(X,Y);
xlabel('Tempo');
ylabel('Graus');
title('Inclinação do pescoço do usuário com relação ao tempo.')
axis tight
