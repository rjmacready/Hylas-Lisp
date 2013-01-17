function toInt(n){ return Math.round(Number(n)); }

function HSV_to_RGB(h,s,v)
{
  var h_i = toInt((h*6));
  var f = h*6 - h_i;
  var p = v * (1 - s);
  var q = v * (1 - f*s);
  var t = v * (1 - (1 - f) * s);
  var r;
  var g;
  var b;
  if(h_i==0)
  { r = v; g = t; b = p; }
  if(h_i==1)
  { r = q; g = v; b = p; }
  if(h_i==2)
  { r = p; g = v; b = t; }
  if(h_i==3)
  { r = p; g = q; b = v; }
  if(h_i==4)
  { r = t; g = p; b = v; }
  if(h_i==5)
  { r = v; g = p; b = q; }
  return [toInt(r*256), toInt(g*256), toInt(b*256)];
}

function colGen()
{
  var color = HSV_to_RGB(Math.random(),0.6,0.8);
  var tmp = "rgb&#40;" + color[0] + "," + color[1] + "," + color[2] + "&#41;;";
  return tmp;
}

var elements = document.getElementsByClassName('Hylas');
for(var i = 0; i < elements.length; i++)
{
  var text = elements[i].innerHTML;
  var inString = false;
  var Colors = [];
  for(var j = 0; j < text.length; j++)
  {
    if((text[j] == '(') && !inString)
    {
      Colors.push(colGen());
      text = text.substr(0,j) + "<span class=\"paren close\" style=\"color:" + Colors[Colors.length-1] + "\"><strong>&#40;</strong></span>" + text.substr(j+1);
    }
    else if((text[j] == ')') && !inString)
      text = text.substr(0,j) + "<span class=\"paren close\" style=\"color:" + Colors.pop() + "\"><strong>&#41;</strong></span>" + text.substr(j+1);
  }
  elements[i].innerHTML = "<span class=\"hylas-code\">" + text + "</span>";
}