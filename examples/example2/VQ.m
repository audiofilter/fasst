function [means,vars,index] = VQ(x,niveau_max)

%
% [means,vars,index] = VQ(x,niveau_max)
%
% quantification vectorielle pour calculer des DSP
%
% input
% -----
%
% x            : matrice n*T du spectrogramme des données
% niveau_max   : nombre de split dans la VQ
%
% output
% ------
%
% means      : matrix n*Q of cluster means, avec Q<=2^niveau_max
% vars       : matrix n*Q of cluster variances, avec Q<=2^niveau_max
% index      : vecteur (2^niveau_max+1)*1 des splits
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                        
% Copyright Laurent Benaroya (Universite Rennes 1), Alexey Ozerov(INRIA)
%                                                                                                  
% This software is distributed under the terms of the GNU Public License                           
% version 3 (http://www.gnu.org/licenses/gpl.txt)                                                  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 

[n,T] = size(x);

index = zeros((2^niveau_max)+1,1);

index(1) = 1;
index(2) = T+1;

new_index = index;

for t=1:niveau_max
  id = 2;
  np = sum(index~=0)-1; % nombre de partitions
  for dd=1:np
    if (index(dd) <(index(dd+1)-1)) % on vérifie qu'il y a plus d'un échantillon dans la partition
      
      % centrage des échantillons d'une partition
      y = x(:,index(dd):(index(dd+1)-1))-mean(x(:,index(dd):(index(dd+1)-1)),2)*ones(1,index(dd+1)-index(dd));
      
      % calcul de la 1ere direction principale
      v = ones(n,1);
      for k=5
	scal = y'*v;
	u = y*(scal);
	v = u/(norm(u)+1e-300);
      end
      
      % on splite les donnees orthogonalement a la direction principale 
      z = v'*y;
      ind1 = find(z>=0);
      ind2 = find(z<0);
      
      % rafinement du splittage
      for it=1:3
	dsp1 = mean(x(:,index(dd)+ind1-1),2);
	dsp2 = mean(x(:,index(dd)+ind2-1),2);
	dist1 = sum((abs(x(:,index(dd):(index(dd+1)-1))-dsp1*ones(1,index(dd+1)-index(dd))) .^ 2),1);
	dist2 = sum((abs(x(:,index(dd):(index(dd+1)-1))-dsp2*ones(1,index(dd+1)-index(dd))) .^ 2),1);
	ind1 = find(dist1<dist2);
	ind2 = find(dist1>=dist2);
      end
      
      if (length(ind1)==0 | length(ind2) ==0)
	new_index(id) = index(dd+1);
	id = id+1;
      else
	x(:,index(dd):(index(dd+1)-1)) = [x(:,index(dd)+ind1-1) x(:,index(dd)+ind2-1)];
	new_index(id) = index(dd)+length(ind1); % insertion du nouveau split
	new_index(id+1) = index(dd+1);
	id = id+2;
      end
    else
      new_index(id) = index(dd+1);
      id = id+1;
    end
    %id
  end
  index = new_index;
end

% calculate means and variances of clusters
np = sum(index~=0)-1; % nombre de partitions
index = zeros(np+1,1);
index(1:(np+1)) = new_index(1:(np+1));
means = zeros(n,np);
vars  = ones(n,np);
for t=1:np
  cur_clust = x(:,index(t):(index(t+1)-1));
  
  means(:,t) = mean(cur_clust,2);
  if size(cur_clust, 2) > 1
    vars(:,t)  = var(cur_clust')';
  else
    vars(:,t)  = 1e-300;
  end;
end
