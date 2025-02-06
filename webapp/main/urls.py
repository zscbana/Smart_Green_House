from django.urls import path, include
from . import views

urlpatterns = [
    path('', views.home, name='main-home'),
    path('about/', views.about, name='main-about'),
    path("roadmap/", views.roadmap, name='main-roadmap'),
]
