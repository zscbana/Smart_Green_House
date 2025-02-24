from django.shortcuts import render

# Create your views here.

def dashboard(request):
    return render(request, 'dashboard/dashboard.html', {'title': 'DashBoards'})

def soil_moisture(request):
    return render(request, 'dashboard/soil_moisture.html', {'title': 'SoilMoisture'})

def temperature(request):
    return render(request, 'dashboard/temperature.html', {'title': 'Temperature'})

def humidity(request):
    return render(request, 'dashboard/humidity.html', {'title': 'Humidity'})
