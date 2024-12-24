var builder = WebApplication.CreateBuilder(args);

var MyAllowSpecificOrigins = "AllowAngularApp";

builder.Services.AddCors(options =>
{
    options.AddPolicy(name: MyAllowSpecificOrigins,
                      policy =>
                      {

                          policy.AllowAnyMethod()   //Allow any HTTP method (GET, POST, etc.)
                                .AllowAnyHeader()   //Allow any header in the request
                                .AllowCredentials() //Allow credentials (cookies, authorization, headers)
                                .WithOrigins("http://localhost:4200/")
                                .SetIsOriginAllowed((host) => true); //Allow all origins, you can restrict this for production


                      });
});

// Add services to the container.

builder.Services.AddControllers();
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

var app = builder.Build();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}
//Configure the middleware for WebSockets 
app.UseWebSockets();
app.UseCors(MyAllowSpecificOrigins);
//app.UseRouting();
//app.UseHttpsRedirection();
//Explicityly configure the app to listen on HTTPS (port 7025)
app.Urls.Add("http://0.0.0.0:5053"); //to specify the url and port
//app.Urls.Add("https://0.0.0.0:7181"); //to specify the url and port
//app.UseAuthentication();
app.UseAuthorization();

app.MapControllers();

app.Run();
