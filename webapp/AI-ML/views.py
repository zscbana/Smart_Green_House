from django.shortcuts import render, redirect
from django.http import HttpResponse
from .models import StudySession
from django.utils import timezone
from datetime import timedelta
from django.db.models import Sum  

def timer(request):
    return render(request, "timer/timer.html", {'title': 'StudySession'})

def status(request):
    today = timezone.now().date()
    start_of_week = today - timedelta(days=today.weekday())
    start_of_month = today.replace(day=1)

    daily_sessions = StudySession.objects.filter(user=request.user, created_at__date=today)
    weekly_sessions = StudySession.objects.filter(user=request.user, created_at__gte=start_of_week)
    monthly_sessions = StudySession.objects.filter(user=request.user, created_at__gte=start_of_month)

    daily_time = daily_sessions.aggregate(Sum('duration'))['duration__sum'] or 0
    weekly_time = weekly_sessions.aggregate(Sum('duration'))['duration__sum'] or 0
    monthly_time = monthly_sessions.aggregate(Sum('duration'))['duration__sum'] or 0

    context = {
        'daily_time': daily_time // 3600,
        'daily_minutes': (daily_time % 3600) // 60,
        'weekly_time': weekly_time // 3600,
        'weekly_minutes': (weekly_time % 3600) // 60,
        'monthly_time': monthly_time // 3600,
        'monthly_minutes': (monthly_time % 3600) // 60,
    }
    return render(request, 'timer/status.html', context)

def save_study_session(request):
    if request.method == 'POST':
        duration = int(request.POST.get('duration', 0))  # Get duration from the POST data
        StudySession.objects.create(user=request.user, duration=duration)
        return redirect('status')  # Redirect to the status page
    return redirect('timer')  # Redirect to timer if not a POST request
