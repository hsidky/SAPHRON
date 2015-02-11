function probability_reweight(x, func, rng)

    f = figure;
    ax = axes('Parent',f,'position',[0.10 0.2 0.80 0.75]);
    beta = mean(rng);
    y = func(beta);
    h = plot(x,y);
    
    b = uicontrol('Parent',f,'Style','slider','Position',[40, 40, 470 ,20],...
                  'Units', 'normalized', 'value', beta, 'min',rng(1), 'max',rng(2), 'SliderStep',[(max(rng)-min(rng))/1000 (max(rng)-min(rng))/1000]);
    bgcolor = f.Color;
    bl1 = uicontrol('Parent',f,'Style','text','Position',[10,37,23,23],...
                    'Units', 'normalized', 'String','0','BackgroundColor',bgcolor);
    bl2 = uicontrol('Parent',f,'Style','text','Position',[520,37,23,23],...
                    'Units', 'Normalized', 'String','1','BackgroundColor',bgcolor);
    bl3 = uicontrol('Parent',f,'Style','text','Position',[180,10,200,23],...
                    'Units', 'normalized', 'String',sprintf('Scaling Parameter = %f',beta),'BackgroundColor',bgcolor);

    function updateData(source,callbackdata)
       set(h, 'YData', func(source.Value));
       set(bl3, 'String', sprintf('Scaling Parameter = %f', source.Value));
    end
                
    set(b,'Callback',@updateData); 
end
 
