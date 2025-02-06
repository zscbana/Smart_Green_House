from typing import Any
from django.contrib.auth.models import User
from django.shortcuts import render, get_object_or_404
# Create your views here.

def home(request):
    return render(request, 'main/home.html')

def about(request):
    return render(request, 'main/about.html', {'title': 'About'})

def roadmap(request):
    return render(request, 'main/ai.html', {'title': 'Roadmap'})

