from django.urls import path
from . import views

urlpatterns = [
    path("", views.timer, name="timer"),
    path("status/", views.status, name="status"),
    path("save/", views.save_study_session, name="save_study_session"),  # Add this line
]
