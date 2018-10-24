
while 1
api = 'http://gabrielvanderlei.000webhostapp.com/cervical/';
url = [api 'loadData.php'];
S = webread(url);
X = [S.time];
Y = [S.angulo];
plot(X,Y);
xlabel('Tempo');
ylabel('Ângulo');
title('Inclinação do pescoço do usuário com relação ao tempo.')
axis tight
pause(1)
end
