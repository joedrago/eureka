with var ball = object()
{
    var x = 10;
    var y = 10;
    var dx = 10;
    var dy = 10;

    function update(self, ms)
    {
        self.x += self.dx;
        self.y += self.dy;
        if((self.x > 800) or (self.x < 10))
        {
            self.dx *= -1;
        }
        if((self.y > 600) or (self.y < 10))
        {
            self.dy *= -1;
        }
    }

    function draw(self)
    {
        drawBox(self.x, self.y, self.x + 10, self.y + 10, 255, 255, 0);
    }
}


function init()
{
    print("Pong initialized!\n");
}

function update(ms)
{
    ball:update(ms);
}

function render()
{
    # Background
    drawBox(0,0,800,600, 64,64,64);

    ball:draw();
}

